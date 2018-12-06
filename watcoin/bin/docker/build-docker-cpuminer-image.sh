#!/usr/bin/env bash
docker build \
    --file watcoin/docker/Dockerfile \
    --tag watcoin:cpuminer \
    .
