#include "../src/hash-backend-finance-challenge.h"

#include <Cutelyst/WSGI/wsgi.h>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>

#include <QObject>
#include <QTest>

class TestApi: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testApi_data();
    void testApi();

private:
    hash_backend_finance_challenge m_app;
    CWSGI::WSGI m_wsgi;
    QNetworkAccessManager m_nam;
};

void TestApi::initTestCase()
{
    m_wsgi.setHttpSocket({ QStringLiteral(":8000") });
    m_wsgi.start(&m_app);
    auto loop = new QEventLoop(this);
    connect(&m_wsgi, &CWSGI::WSGI::ready, this, [=] {
        loop->quit();
    });
    loop->exec();
}

void TestApi::testApi_data()
{
    QTest::addColumn<QByteArray>("method");
    QTest::addColumn<QString>("url");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QJsonObject>("output");

    QTest::newRow("api-00") << QByteArrayLiteral("GET") << QStringLiteral("/api/v1/corrida") << 200
                            << QJsonObject{ {QStringLiteral("user_name"), QStringLiteral("")}, {QStringLiteral("price_total"), 0} };

    QTest::newRow("api-01") << QByteArrayLiteral("GET") << QStringLiteral("/api/v1/corrida?distance=10&minutes=5&city=Salvador") << 200
                            << QJsonObject{ {QStringLiteral("user_name"), QStringLiteral("")}, {QStringLiteral("price_total"), 8.45} };

    QTest::newRow("api-02") << QByteArrayLiteral("GET") << QStringLiteral("/api/v1/corrida?distance=10&minutes=5&city=Salvador") << 200
                            << QJsonObject{ {QStringLiteral("user_name"), QStringLiteral("")}, {QStringLiteral("price_total"), 8.65} };

    QTest::newRow("api-03") << QByteArrayLiteral("GET") << QStringLiteral("/api/v1/corrida?distance=10&minutes=5&city=Salvador&user_id=1") << 200
                            << QJsonObject{ {QStringLiteral("user_name"), QStringLiteral("Fulano")}, {QStringLiteral("price_total"), 8.85} };
}

void TestApi::testApi()
{
    QFETCH(QByteArray, method);
    QFETCH(QString, url);
    QFETCH(int, statusCode);
    QFETCH(QJsonObject, output);

    QNetworkRequest req(QLatin1String("http://localhost:8000") + url);
    QNetworkReply *reply = m_nam.sendCustomRequest(req, method);
    auto loop = new QEventLoop(reply);
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();
        loop->quit();

        QCOMPARE(reply->error(), QNetworkReply::NoError);

        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), statusCode);

        QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
        QVERIFY2(obj == output, QJsonDocument(obj).toJson());
    });
    loop->exec();
}

QTEST_MAIN(TestApi)

#include "hf.moc"
