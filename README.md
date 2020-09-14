# Hash financial challenge

Build & run - MUST change DB_URI env var to a valid postgredb 

    docker build -t hf:v1 .
    docker run -e DB_URI=postgres://user:pass@host/dbname -p 127.0.0.1:3000:3000/tcp -ti hf:v1

