FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    bash \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY build/bin/kvspp-cli /app/kvspp-cli
RUN chmod +x /app/kvspp-cli

# Add /app to PATH so kvspp-cli can be run from anywhere
ENV PATH="/app:${PATH}"

# Default entrypoint is a shell, so the container stays alive and you can exec commands
ENTRYPOINT ["/bin/bash"]