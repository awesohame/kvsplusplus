# Use Ubuntu as the base image
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the CLI executable from build output
COPY build/bin/kvspp-cli.exe /app/kvspp-cli.exe

# Set the entrypoint to the CLI tool
ENTRYPOINT ["/app/kvspp-cli.exe"]
