#ifndef APIV1_H
#define APIV1_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ApiV1 : public Controller
{
    Q_OBJECT
public:
    explicit ApiV1(QObject *parent = nullptr);

    C_ATTR(corrida, :Local :ActionClass(REST) :AutoArgs)
    void corrida(Context *c);

    C_ATTR(corrida_GET, :Private)
    void corrida_GET(Context *c);
};

#endif // APIV1_H
