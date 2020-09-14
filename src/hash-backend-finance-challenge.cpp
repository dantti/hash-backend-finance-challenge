#include "hash-backend-finance-challenge.h"

#include "apiv1.h"
#include "amigrations.h"

#include <apool.h>
#include <aresult.h>

#include <QTimer>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(H_MAIN, "hashf.main", QtInfoMsg)

using namespace Cutelyst;

hash_backend_finance_challenge::hash_backend_finance_challenge(QObject *parent) : Application(parent)
{
}

hash_backend_finance_challenge::~hash_backend_finance_challenge()
{
}

bool hash_backend_finance_challenge::init()
{
    new ApiV1(this);

    return true;
}

bool hash_backend_finance_challenge::postFork()
{
    const QString defaultUri = qEnvironmentVariable("DB_URI", QStringLiteral("postgres:///hashf"));
    const auto dbUri = config(QStringLiteral("db_uri"), defaultUri).toString();
    qInfo(H_MAIN) << "Creating pool" << dbUri;
    APool::addDatabase(dbUri);

    setupDecreaseDemand();

    auto mig = new AMigrations(this);
    mig->fromString(QStringLiteral(R"V0G0N(
                                   -- 1 up
                                   CREATE SCHEMA hf;
                                   -- 1 down
                                   DROP SCHEMA hf;

                                   -- 2 up
                                   CREATE TABLE hf.users (
                                     id SERIAL PRIMARY KEY,
                                     name text NOT NULL
                                   );
                                   INSERT INTO hf.users (name) VALUES ('Fulano');
                                   INSERT INTO hf.users (name) VALUES ('Ciclano');
                                   -- 2 down
                                   DROP TABLE hf.users;

                                   -- 3 up
                                   CREATE TABLE hf.cities (
                                     id SERIAL PRIMARY KEY,
                                     name text NOT NULL,
                                     price_base numeric(15,2) NOT NULL,
                                     price_minute numeric(15,2) NOT NULL,
                                     price_km numeric(15,2) NOT NULL,
                                     price_tax numeric(15,2) NOT NULL,
                                     demand_multiplier numeric(15,2) NOT NULL DEFAULT 1
                                   );
                                   INSERT INTO hf.cities (name, price_base, price_minute, price_km, price_tax) VALUES
                                   ('São Paulo', 3.50, 1.00, 0.50, 0.75),
                                   ('Salvador', 1.50, 0.75, 0.20, 1.20),
                                   ('Curitiba', 2.00, 0.80, 0.50, 1.00),
                                   ('Rio de Janeiro', 0.95, 0.60, 0.50, 1.40);
                                   -- 3 down
                                   DROP TABLE hf.cities;

                                   -- 4 up
                                   CREATE TABLE hf.city_demand (
                                     created_dt timestamp with time zone NOT NULL DEFAULT now(),
                                     city_id integer REFERENCES hf.cities(id)
                                   );
                                   CREATE OR REPLACE FUNCTION hf.city_demand_increase()
                                     RETURNS trigger AS $$
                                   BEGIN
                                     UPDATE hf.cities SET demand_multiplier = demand_multiplier + 0.1 WHERE id = NEW.city_id;
                                     RETURN NEW;
                                   END;
                                   $$ LANGUAGE plpgsql;
                                   CREATE OR REPLACE FUNCTION hf.city_demand_decrease()
                                     RETURNS trigger AS $$
                                   BEGIN
                                     UPDATE hf.cities SET demand_multiplier = demand_multiplier - 0.1 WHERE id = OLD.city_id;
                                     RETURN NEW;
                                   END;
                                   $$ LANGUAGE plpgsql;
                                   CREATE TRIGGER city_demand_increase
                                     AFTER INSERT ON hf.city_demand
                                     FOR EACH ROW
                                     EXECUTE PROCEDURE hf.city_demand_increase();
                                   CREATE TRIGGER city_demand_decrease
                                     AFTER DELETE ON hf.city_demand
                                     FOR EACH ROW
                                     EXECUTE PROCEDURE hf.city_demand_decrease();
                                   -- 4 down
                                   DROP TABLE hf.city_demand;

                                   -- 5 up
                                   CREATE OR REPLACE FUNCTION hf.get_price(v_distance integer, v_minutes integer, v_city text, v_user_id integer)
                                   RETURNS TABLE (
                                       user_name text,
                                       price_total numeric(15,2)
                                   ) AS $$
                                   DECLARE
                                   BEGIN
                                       RETURN QUERY SELECT
                                       (SELECT u.name FROM hf.users u WHERE u.id = v_user_id),
                                       (SELECT c.price_base + ((c.price_minute * v_minutes) + (c.price_km * v_distance) * c.demand_multiplier) + c.price_tax
                                        FROM hf.cities c
                                        WHERE c.name = v_city);

                                       INSERT INTO hf.city_demand (city_id) SELECT id FROM hf.cities c WHERE c.name = v_city;
                                   END; $$
                                   LANGUAGE PLPGSQL;
                                   -- 5 down
                                   DROP FUNCTION hf.get_price ( integer, integer, text, integer) ;

                                   )V0G0N"));
    connect(mig, &AMigrations::ready, this, [=] (bool error, const QString &errorString) {
        if (error) {
            qCritical(H_MAIN) << "Failed to check db version" << errorString;
            return;
        }

        mig->migrate([=] (bool error, const QString &errorString) {
            if (error) {
                qCritical(H_MAIN) << "Failed to migrate" << errorString;
                return;
            }
            qInfo(H_MAIN) << "Database at version" << mig->active();
        });
    });
    mig->load(APool::database(), QStringLiteral("hashf"));

    return true;
}

void hash_backend_finance_challenge::setupDecreaseDemand()
{
    const qint64 msec = QDateTime::currentMSecsSinceEpoch();
    const int ms = 60000 - (QDateTime::currentMSecsSinceEpoch() % 60000);
    qInfo(H_MAIN) << "next tick" << QDateTime::fromMSecsSinceEpoch(msec) << msec << msec % 60000 << ms;

    auto t = new QTimer(this);
    t->setTimerType(Qt::PreciseTimer);
    t->setSingleShot(true);
    connect(t, &QTimer::timeout, this, [=] {
        const qint64 msec = QDateTime::currentMSecsSinceEpoch();

        APool::database().exec(QStringLiteral("DELETE FROM hf.city_demand WHERE created_dt < now() - INTERVAL '5 minutes'"),
                [=] (AResult &result) {
            if (result.error()) {
                qCritical(H_MAIN) << "Failed to retrieve data" << result.errorString();
            }
        }, this);

        const int ms = 60000 - (QDateTime::currentMSecsSinceEpoch() % 60000);
        qInfo(H_MAIN) << "timeout at" << QDateTime::fromMSecsSinceEpoch(msec) << "next in" << ms;
        t->start(ms);
    });
    t->start(ms);
}
