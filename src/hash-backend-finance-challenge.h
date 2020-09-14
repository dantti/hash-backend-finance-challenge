#ifndef HASH_BACKEND_FINANCE_CHALLENGE_H
#define HASH_BACKEND_FINANCE_CHALLENGE_H

#include <Cutelyst/Application>

using namespace Cutelyst;

class hash_backend_finance_challenge : public Application
{
    Q_OBJECT
    CUTELYST_APPLICATION(IID "hash_backend_finance_challenge")
public:
    Q_INVOKABLE explicit hash_backend_finance_challenge(QObject *parent = nullptr);
    ~hash_backend_finance_challenge();

    bool init() override;
    bool postFork() override;

private:
    void setupDecreaseDemand();
};

#endif //HASH_BACKEND_FINANCE_CHALLENGE_H

