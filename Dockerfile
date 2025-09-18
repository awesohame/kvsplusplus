FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY build/bin/kvspp-cli /app/kvspp-cli
ENTRYPOINT ["/app/kvspp-cli"]