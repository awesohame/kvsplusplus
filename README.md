# KVS++ (Key Value Store++)

A high-performance, thread-safe, in-memory key-value store with Redis-like functionality, implemented in modern C++. KVS++ provides a robust foundation for caching, session storage, and fast data retrieval with comprehensive CLI tools for both interactive use and automation.

## Features

### Core Functionality
- **Thread-Safe Operations**: All store operations are thread-safe with minimal locking overhead
- **Type System**: Strong typing with automatic type validation and global type registry
- **Persistence**: JSON-based save/load functionality with atomic operations
- **Fast Lookups**: O(1) key access with efficient in-memory data structures
- **Search Capabilities**: Type-aware attribute searching with exact match support

### Data Types
- **String**: Text values with UTF-8 support
- **Integer**: 64-bit signed integers
- **Double**: IEEE 754 double-precision floating-point
- **Boolean**: true/false values with type safety

### CLI Tools
- **Interactive CLI**: REPL-style interface for exploration and development
- **Single Command CLI**: For scripting and automation
- **Demo Application**: Showcase functionality with sample data
- **Test Suite**: Unit tests for validation

## Quick Start

### Building the Project
All commands below are for Windows powershell terminal

```powershell
# Clone and build
git clone https://github.com/awesohame/kvsplusplus.git
cd kvsplusplus
mkdir build
cd build
cmake ..
cmake --build .

# All executables will be in build/bin/
```


### Basic Usage

> **Note:** All commands now require a `storeToken` (store ID) as the first argument to identify which logical store to use. This enables multiple isolated stores in a single process.

```powershell
# Interactive mode
.\build\bin\kvspp-cli.exe

# Single commands (with storeToken)
.\build\bin\kvspp-single-cmd.exe put mystore user1 name:John age:25 active:true
.\build\bin\kvspp-single-cmd.exe get mystore user1
.\build\bin\kvspp-single-cmd.exe search mystore age 25

# Run demo
.\build\bin\kvspp-demo.exe

# Run tests
.\build\bin\kvspp-test.exe
```
## Multi-Store Support

KVS++ now supports multiple isolated stores, each identified by a `storeToken` (a string you provide). All commands must specify the store token as the first argument:

```powershell
# Put and get in different stores
.\build\bin\kvspp-single-cmd.exe put store1 keyA value:foo
.\build\bin\kvspp-single-cmd.exe put store2 keyA value:bar
.\build\bin\kvspp-single-cmd.exe get store1 keyA   # returns foo
.\build\bin\kvspp-single-cmd.exe get store2 keyA   # returns bar
```

Each store is persisted separately as `store/<storeToken>.json`.

## Architecture

### Project Structure
```
kvsplusplus/
├── src/
│   ├── core/           # Core key-value store implementation
│   ├── cli/            # CLI framework and command processing
│   ├── persistence/    # JSON save/load functionality
│   ├── utils/          # Helper utilities and terminal colors
│   ├── cli_main.cpp    # Interactive CLI entry point
│   ├── single_cmd_main.cpp  # Single command CLI entry point
│   ├── demo_main.cpp   # Demo application entry point
│   └── tests_main.cpp  # Test suite entry point
├── include/            # Public headers
├── CLI_README.md       # Detailed CLI documentation
└── build/bin/   # Generated executables (after you compile)
```

### Core Components

- **KeyValueStore**: Thread-safe main store with attribute-based entries
- **ValueObject**: Type-safe value container with automatic type conversion
- **TypeRegistry**: Global singleton ensuring type consistency across the store
- **PersistenceManager**: JSON serialization/deserialization with error handling
- **CLI Framework**: Colored output, command parsing, and user interaction

## Executables

| Executable | Purpose | Use Case |
|------------|---------|----------|
| `kvspp-single-cmd.exe` | Single command CLI | Scripting, automation, CI/CD |
| `kvspp-cli.exe` | Interactive CLI | Use tool through CLI |
| `kvspp-demo.exe` | Demo application | Feature showcase, sample data |
| `kvspp-test.exe` | Test suite | for devs (development testing) |

## Documentation

- **[CLI Documentation](CLI_README.md)** - Comprehensive CLI usage guide
- **[API Reference](#)** - Core library API documentation (TODO)
- **[Examples](#)** - More usage examples and patterns (TODO)

## Advanced Features

### JSON Output Mode
Perfect for automation and scripting:
```powershell
# Get structured data for processing
$userData = .\build\bin\kvspp-single-cmd.exe --json get user1 | ConvertFrom-Json
Write-Host "User age: $($userData.age)"
```


### Custom Store Files (Legacy)
Previously, you could use the `-f` flag to specify a custom store file. Now, use the `storeToken` argument for logical separation. For backward compatibility, the `-f` flag will map to a store token based on the filename.


### Batch Operations
Load and process data efficiently:
```powershell
# Load demo data into a specific store
.\build\bin\kvspp-single-cmd.exe load mystore store/demo_store.json
$premiumUsers = .\build\bin\kvspp-single-cmd.exe --json search mystore premium true | ConvertFrom-Json
```

## Use Cases

- **Application Caching**: Fast in-memory cache for web applications
- **Session Storage**: User session data with type safety
- **Configuration Management**: Typed configuration values with persistence
- **Data Processing**: Fast lookup tables for data transformation
- **Development Tools**: Quick data storage for prototyping and testing

## Performance

- **Memory Efficient**: Optimized data structures with minimal overhead
- **Thread Safe**: Concurrent access support without performance degradation
- **Fast Operations**: O(1) key operations, O(n) search with efficient implementation
- **Scalable**: Handles vast number of entries efficiently in memory

## Status

### Completed
- Core key-value operations (get, put, delete, search)
- Type system with validation
- Thread-safe implementation
- JSON persistence
- Complete CLI tools
- Comprehensive test suite

### TODO
- Authentication and access control
- Encryption
- Configuration file support
- Advanced search (multi-attribute queries)
- CSV import/export
- Basic logging and metrics
- Network mode (TCP server)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by Redis for the key-value store concept
- Built with modern C++ best practices
- CLI framework inspired by popular command-line tools

---
