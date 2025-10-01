# KVS++  TCP Protocol

## Usage
Start server (default port 5555):
```
kvspp-tcp.exe [port]
```

## Commands
- `SELECT <storetoken>`: Choose store for session
- `AUTOSAVE ON|OFF`: Toggle autosave
- `SET <key> <value>`: Set key
- `GET <key>`: Get value
- `DELETE <key>`: Delete key
- `SAVE <filename>`: Save store
- `LOAD <filename>`: Load store
- `QUIT`: Disconnect

## Responses
- `OK`: Success
- `VALUE <value>`: GET result
- `NOT_FOUND`: Key missing
- `ERROR <message>`: Error
