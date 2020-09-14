FROM ubuntu:20.04

ENV TZ=America/Sao_Paulo
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y wget
RUN wget https://github.com/cutelyst/cutelyst/releases/download/v2.12.0/cutelyst_2.12.0_amd64.deb && apt install -y ./cutelyst_2.12.0_amd64.deb
RUN wget https://github.com/cutelyst/asql/releases/download/v0.13.0/libasql_0.13.0_amd64.deb && apt install -y ./libasql_0.13.0_amd64.deb

RUN wget https://github.com/dantti/hash-backend-finance-challenge/releases/download/v0.1.0/hash_backend_finance_challenge_0.1.0_amd64.deb && apt install -y ./hash_backend_finance_challenge_0.1.0_amd64.deb

ENV DB_URI=postgres:///hashf

EXPOSE 3000
CMD /usr/bin/cutelyst-wsgi2 -a /usr/lib/x86_64-linux-gnu/libhash_backend_finance_challenge --h1 :3000
