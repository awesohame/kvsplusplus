# KVS++ CLI Documentation

## Overview

The KVS++ project now includes comprehensive CLI tools for interacting with the in-memory key-value store. The implementation provides three main executables:

## Executables

### 1. `kvspp-single-cmd.exe` - Single Command CLI
**Purpose**: Execute single commands for scripting and automation
**Usage**: `kvspp-single-cmd [OPTIONS] <command> [args...]`

#### Options
- `-h, --help`: Show help message
- `-v, --verbose`: Enable verbose output
- `-j, --json`: Enable JSON output mode
- `-f, --file FILE`: Use FILE as the store file (default: store/store.json)
- `--no-autosave`: Disable automatic saving after command

#### Examples
```powershell
# Store a new entry
.\build\bin\Release\kvspp-single-cmd.exe put user1 name:John age:25 active:true

# Get an entry
.\build\bin\Release\kvspp-single-cmd.exe get user1

# Search for entries
.\build\bin\Release\kvspp-single-cmd.exe search age 25

# JSON output for scripting
.\build\bin\Release\kvspp-single-cmd.exe --json keys

# Use custom store file
.\build\bin\Release\kvspp-single-cmd.exe -f mystore.json keys
```

### 2. `kvspp-cli.exe` - Interactive CLI
**Purpose**: Interactive REPL-style interface for exploration and development
**Usage**: `kvspp-cli [OPTIONS]`

#### Features
- Interactive command prompt
- Command history
- Colored output for better readability
- Auto-save on exit
- Same command set as single command CLI

#### Example Session
```
=== Welcome to KVS++ Store CLI ===
kvs++ > put course title:"Webdev" level:beginner duration:20
[SUCCESS] Successfully stored key 'course' with 3 attributes

kvs++ > get course
course -> title: Webdev, level: beginner, duration: 20

kvs++ > search level beginner
[INFO] Found 1 keys:
  course

kvs++ > exit
[INFO] Goodbye!
```

### 3. `kvspp-demo.exe` - Demo Application
**Purpose**: Showcase functionality with sample data
**Usage**: `kvspp-demo`

Creates sample e-commerce course data and user profiles, demonstrates all core functionality, and saves demo data to `store/demo_store.json`.

### 4. `kvspp-test.exe` - Test Suite
**Purpose**: Run the comprehensive test suite
**Usage**: `kvspp-test`

## Available Commands

### Data Operations
- `get <key>` - Get value for a key
- `put <key> <attr:val> ...` - Store key with attributes
- `delete <key>` - Delete a key  
- `search <attr> <value>` - Find keys by attribute

### Store Operations
- `keys` - List all keys
- `clear` - Clear all data
- `stats` - Show store statistics
- `inspect <key>` - Detailed view of a key

### File Operations
- `save [filename]` - Save store to file
- `load [filename]` - Load store from file

### Utility
- `help` - Show command help
- `exit/quit/q` - Exit (interactive mode only)

## Key Features Implemented

### Core Functionality
- **Thread-safe operations**: All store operations are thread-safe
- **Type consistency**: Attribute types are enforced across the store
- **Persistence**: JSON-based save/load functionality
- **Error handling**: Comprehensive exception handling with user-friendly messages

### CLI Features
- **Multiple interfaces**: Single command and interactive modes
- **JSON output**: Machine-readable output for scripting
- **Colored output**: Enhanced readability with ANSI colors
- **Auto-save**: Automatic persistence on exit
- **Verbose mode**: Detailed operation logging
- **Flexible file handling**: Custom store file locations

### Data Types Supported
- **String**: Text values
- **Integer**: Whole numbers
- **Double**: Floating-point numbers  
- **Boolean**: true/false values

### Search Capabilities
- **Exact match**: Find keys by exact attribute value
- **Type-aware**: Searches respect type consistency
- **Fast lookup**: Efficient search implementation

## Advanced Usage Examples

### Scripting with JSON Output
```powershell
# Get all keys as JSON array
$keys = .\build\bin\Release\kvspp-single-cmd.exe --json keys | ConvertFrom-Json

# Get specific entry as JSON object
$user = .\build\bin\Release\kvspp-single-cmd.exe --json get user1 | ConvertFrom-Json
Write-Host "User age: $($user.age)"

# Search and process results
$results = .\build\bin\Release\kvspp-single-cmd.exe --json search active true | ConvertFrom-Json
Write-Host "Found $($results.Length) active users"
```

### Batch Operations
```powershell
# Load demo data
.\build\bin\Release\kvspp-single-cmd.exe load store/demo_store.json

# Process all premium users
$premiumUsers = .\build\bin\Release\kvspp-single-cmd.exe --json search premium true | ConvertFrom-Json
foreach ($user in $premiumUsers) {
    $userData = .\build\bin\Release\kvspp-single-cmd.exe --json get $user | ConvertFrom-Json
    Write-Host "Premium user: $($userData.name), Age: $($userData.age)"
}
```

### Custom Store Management
```powershell
# Create a custom store for different data
.\build\bin\Release\kvspp-single-cmd.exe -f products.json put product1 name:"Laptop" price:999.99 stock:50
.\build\bin\Release\kvspp-single-cmd.exe -f products.json put product2 name:"Mouse" price:29.99 stock:200

# List products
.\build\bin\Release\kvspp-single-cmd.exe -f products.json keys
```

## Architecture Highlights

### Modular Design
- **Core Library**: Reusable `libkvstore.a` for all executables
- **CLI Framework**: Flexible command processing with colored output
- **Clean Separation**: Business logic separated from presentation

### Type System
- **Global Registry**: Singleton TypeRegistry ensures consistency
- **Runtime Validation**: Type mismatches are caught and reported
- **Automatic Parsing**: String inputs automatically converted to appropriate types

### Persistence Layer
- **JSON Format**: Human-readable storage format
- **Atomic Operations**: Safe save/load with error recovery
- **Flexible Paths**: Support for custom file locations

## Error Handling

The CLI provides user-friendly error messages for common scenarios:

```
# Type mismatch
kvs++ > put user1 age:25
kvs++ > put user2 age:notanumber
[ERROR] KVStore Error: Type mismatch for attribute 'age': expected integer, got string

# Key not found
kvs++ > get nonexistent
[ERROR] Key 'nonexistent' not found

# Invalid command syntax
kvs++ > put user1
[ERROR] Usage: put <key> <attr1:value1> [attr2:value2] ...
```

## Performance Characteristics

- **Memory Efficient**: In-memory storage with efficient data structures
- **Fast Lookups**: O(1) key access, O(n) search operations
- **Thread Safe**: Concurrent access support with minimal locking
- **Scalable**: Handles thousands of entries efficiently

## Status and Future Enhancements

### Completed
- Interactive and single command modes
- All core KV operations (get, put, delete, search, keys)
- JSON output mode for scripting
- Colored output and user-friendly interface
- File operations (save, load)
- Statistics and inspection commands

### TODO
- **Configuration files**: User preferences and defaults
- **Advanced search**: Multi-attribute queries and ranges
- **Batch import/export**: CSV and JSON file import
- **Basic authentication**: User access control
- **Command aliases**: User-defined shortcuts
- **Enhanced logging**: Operation audit trail
- **Network mode**: Basic client-server architecture

## Building and Installation

```powershell
# Build all executables
cd d:\Programming\cpp\kvcplusplus
cmake --build build --clean-first

# All executables will be in build/bin/Release/
# - kvspp-single-cmd.exe (single command)
# - kvspp-cli.exe (interactive)
# - kvspp-demo.exe (demo)
# - kvspp-test.exe (test suite)
```

The KVS++ CLI implementation provides a production-ready interface for the Redis-like key-value store with comprehensive features for both interactive use and automation scripting.
