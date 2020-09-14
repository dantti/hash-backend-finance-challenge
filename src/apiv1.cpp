#include "apiv1.h"

#include <apool.h>
#include <aresult.h>

#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(H_API1, "hashf.api.v1", QtInfoMsg)

using namespace Cutelyst;

ApiV1::ApiV1(QObject *parent)
    : Controller(parent)
{
}

void ApiV1::corrida(Context *c)
{
    Q_UNUSED(c)
}

void ApiV1::corrida_GET(Context *c)
{
    c->detachAsync();

    const ParamsMultiMap params = c->request()->queryParams();

    APool::database().exec(QStringLiteral("SELECT user_name, price_total FROM hf.get_price($1, $2, $3, $4)"),
            {params[QLatin1String("distance")].toInt(), params[QLatin1String("minutes")].toInt(),
             params[QLatin1String("city")], params[QLatin1String("user_id")].toInt()},
            [=] (AResult &result) {
        if (result.error()) {
            qCritical(H_API1) << "Failed to retrieve data" << result.errorString();
            c->response()->setJsonObjectBody({ { QStringLiteral("error_msg"), result.errorString()} });
            c->response()->setStatus(Response::InternalServerError);
            c->attachAsync();
            return;
        }

        c->response()->setJsonObjectBody(result.jsonObject());
        c->attachAsync();
    }, c);
}
