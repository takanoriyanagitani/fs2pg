FROM postgres:9.5 AS builder
RUN apt-get update
RUN apt-get install -y gcc
RUN apt-get install -y postgresql-server-dev-all

WORKDIR /build-fs2pg
COPY ./id2path.c ./
RUN gcc -v -o id2path.so -shared -fPIC -O3 id2path.c -I$(pg_config --includedir-server)

FROM postgres:9.5
WORKDIR /etc/postgresql/id2path
COPY --from=builder /build-fs2pg/id2path.so ./
