FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -y gcc make && \
    apt-get clean

WORKDIR /app
#mejorar los archivos que se copian y se mantienen en el contenedor
# integrar con docker hub
COPY . .

RUN make prod

EXPOSE 7986

CMD ["./bin/mochiladb"]
