# KVS++ CLI Documentation

## Usage
Start CLI interface:
```powershell
.\build\bin\kvspp-cli.exe
```

### Example Session
```
kvs++ > put foo value:bar
[SUCCESS] Stored key 'foo'
kvs++ > get foo
foo -> value: bar
kvs++ > keys
foo
kvs++ > exit
[INFO] Goodbye!
```

## Commands (CLI)

### Data Operations
- `get <key>`: Get value for a key
- `put <key> <value>`: Store key with value
- `delete <key>`: Delete a key
- `search <attr> <value>`: Find keys by attribute value *(planned feature)*


### Store Operations
- `keys`: List all keys
- `clear`: Clear all data *(planned feature)*
- `stats`: Show store statistics *(planned feature)*
- `autosave on|off`: Enable/disable autosave

### File Operations
- `save [filename]`: Save store to file
- `load [filename]`: Load store from file

### Utility
- `help`: Show command help
- `exit/quit/q`: Exit CLI


## TCP Protocol
See [TCP_PROTOCOL.md](TCP_PROTOCOL.md) for concise network command documentation.
