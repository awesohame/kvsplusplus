# KVS++ (Key Value Store in C++)

A modern, high-performance, multi-store key-value system in C++ with CLI, TCP server, autosave, and Docker support. KVS++ is designed for fast, robust, and scriptable data storage, with features tailored for automation, integration, and extensibility.

## Features

### Core Functionality
- **Multi-Store Architecture**: Isolated stores identified by a `storeToken`, supporting concurrent access and persistence.
- **Thread-Safe Operations**: All store operations are thread-safe.
- **Flat Value Model**: Each key maps to a flat string value for simplicity and performance.
- **JSON Persistence**: Each store is saved/loaded as a separate JSON file (`store/<storeToken>.json`).
- **Autosave**: Per-store autosave option, persisted in JSON and controllable via CLI and TCP.
- **Fast Lookups**: O(1) key access with efficient in-memory data structures.

### TCP Server
- **Redis-like Protocol**: Supports commands for GET, SET, DELETE, KEYS, AUTOSAVE, SAVE, LOAD, JSON, QUIT, and more.
- **Stream-Safe Command Handling**: Handles partial and split commands robustly.
- **Single-Line JSON Output**: Returns the entire store as a single-line JSON for easy integration with other systems.
- **Multi-Client Support**: Each TCP connection can select and operate on any store.
- See [TCP Protocol Documentation](TCP_PROTOCOL.md) for details.

### CLI Tools
- **Interactive CLI**: REPL-style interface for exploration and development.
- **Single Command CLI**: For scripting and automation.
- **Demo Application**: Showcase functionality with sample data.
- **Test Suite**: Unit tests for validation.
- See [CLI_README.md](CLI_README.md) for full CLI usage.

### Docker Support
- **Portable Builds**: Dockerfile for building and running CLI/TCP server in containers.
- **Volume Mounts**: Persist store data on host via Docker volumes.
- **Cross-Platform**: Build and run on Windows, Linux, WSL, or in CI/CD pipelines.
- **Official image**: `docker pull awesohame/kvspp-tcp:latest`

## Quick Start

### Building the Project
#### Windows (PowerShell)
```powershell
git clone https://github.com/awesohame/kvsplusplus.git
cd kvsplusplus
mkdir build
cd build
cmake ..
cmake --build .
# All executables will be in build/bin/
```

#### Linux/WSL
```sh
git clone https://github.com/awesohame/kvsplusplus.git
cd kvsplusplus
mkdir build
cd build
cmake ..
make -j$(nproc)
# All executables will be in build/bin/
```

### Basic Usage

#### CLI
```powershell
# Interactive mode
.\build\bin\kvspp-cli.exe
```

#### TCP Server
```powershell
# Start TCP server (default port 5555)
.\build\bin\kvspp-tcp.exe

# Connect and run commands (example with ncat)
ncat 127.0.0.1 5555
SELECT mystore
SET foo bar
GET foo
JSON
QUIT
```
See [TCP Protocol Documentation](TCP_PROTOCOL.md) for full command list and protocol details.

## Multi-Store Support

KVS++ supports multiple isolated stores, each identified by a `storeToken` (string). All commands must specify the store token as the first argument. Each store is persisted separately as `store/<storeToken>.json`.

## Architecture

### Project Structure
```
kvsplusplus/
├── src/
│   ├── core/           # Core key-value store implementation
│   ├── cli/            # CLI framework and command processing
│   ├── persistence/    # JSON save/load functionality
│   ├── net/            # TCP server implementation
│   ├── utils/          # Helper utilities
│   ├── cli_main.cpp    # Interactive CLI entry point
│   ├── tcp_main.cpp    # TCP server entry point
├── include/            # Public headers
├── CLI_README.md       # CLI and TCP protocol documentation
├── TCP_PROTOCOL.md     # TCP protocol documentation
└── build/bin/          # Generated executables (after you compile)
```

### Core Components
- **KeyValueStore**: Thread-safe main store with flat value entries
- **PersistenceManager**: JSON serialization/deserialization
- **StoreManager**: Multi-store management and autosave logic
- **TCPServer**: Network protocol and command handling
- **CLI Framework**: Command parsing and user interaction


## Executables

| Executable      | Purpose           | Use Case                |
|-----------------|-------------------|-------------------------|
| `kvspp-cli.exe` | Interactive CLI   | Manual use, scripting   |
| `kvspp-tcp.exe` | TCP server        | Networked key-value API |


## Documentation & Docker Usage

- **[CLI](CLI_README.md) & [TCP](TCP_PROTOCOL.md)** – Full command reference and protocol details

### Docker Usage
Run CLI or TCP server in a portable Docker container (Ubuntu 24.04) using the provided Dockerfile. Useful for deployment, integration, or running on any system with Docker installed.

#### Build the Docker Image
```sh
docker build -t kvspp-tcp .
```

#### Run in Docker (with persistent store directory)
```powershell
# PowerShell (Windows):
docker run -it --rm -v "D:\Programming\cpp\kvsplusplus\store:/app/store" kvspp-tcp
```
```sh
# Linux/WSL:
docker run -it --rm -v "$PWD/store:/app/store" kvspp-tcp
```

- The `store` directory on your host will be mounted into the container, so all data is persistent and accessible.
- The Dockerfile is kept in sync with the latest binary and project files.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by Redis for the key-value store concept
- Built with modern C++ best practices