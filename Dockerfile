FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    bash \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the TCP server executable
COPY build/bin/kvspp-tcp /app/kvspp-tcp
RUN chmod +x /app/kvspp-tcp

# Add /app to PATH so kvspp-tcp can be run from anywhere
ENV PATH="/app:${PATH}"

# Expose the default TCP port
EXPOSE 5555

# Run the TCP server by default
ENTRYPOINT ["/app/kvspp-tcp"]