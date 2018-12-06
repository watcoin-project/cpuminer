#
# Dockerfile for cpuminer
# usage: docker run creack/cpuminer --url xxxx --user xxxx --pass xxxx
# ex: docker run creack/cpuminer --url stratum+tcp://ltc.pool.com:80 --user creack.worker1 --pass abcdef
#
#

FROM ubuntu:18.04
# Install extra tools
RUN apt-get update && \
    apt-get install -y curl vim wget && \
    rm -rf /var/lib/apt/lists/*
# Install git
RUN apt-get update && \
    apt-get install -y git && \
    rm -rf /var/lib/apt/lists/*
# Install basic dependencies
RUN apt-get update && \
    apt-get install -y automake gcc libcurl4-openssl-dev make python3 && \
    rm -rf /var/lib/apt/lists/*
# Copy whole source
COPY . /cpuminer
# Change workdir
WORKDIR /cpuminer
# Configure solution
RUN ./autogen.sh && \
    ./configure CFLAGS="-O3"
# Make solution
RUN make
# Install solution
RUN make install
# Change entrypoint
COPY ./watcoin/bin/docker-cpuminer-entrypoint.sh /docker-cpuminer-entrypoint.sh
ENTRYPOINT ["/docker-cpuminer-entrypoint.sh"]
