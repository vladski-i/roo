FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update && apt upgrade  && apt install -y gcc libjack-jackd2-dev make

ENTRYPOINT ["make", "-C", "/roo"]