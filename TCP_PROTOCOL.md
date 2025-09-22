
# kvspp TCP Protocol (Text-Based)

## Overview
Each TCP connection must first select a store using:

```
SELECT <storetoken>
```

All subsequent commands operate on the selected store for that connection.

## Starting the TCP Server

By default, the server listens on port 5555. To specify a different port, provide it as a command-line argument:

```
kvspp-tcp.exe <port>
```

Example (start on port 6000):

```
kvspp-tcp.exe 6000
```

## Commands

- `SELECT <storetoken>`: Selects the store for this connection (required before other commands).
- `AUTOSAVE ON|OFF`: Enable or disable autosave after SET/DELETE (default: ON).
- `SET <key> <value>`: Set a value for a key.
- `GET <key>`: Get the value for a key.
- `DELETE <key>`: Delete a key.
- `SAVE <filename>`: Save the selected store to `/store/<filename>.json` (relative to project root; `.json` is auto-appended if missing).
- `LOAD <filename>`: Load the selected store from `/store/<filename>.json`.
- `QUIT`: Close the connection.

## Responses

- `OK`: Operation succeeded.
- `VALUE <value>`: Returned for GET if the key exists.
- `NOT_FOUND`: Key does not exist.
- `ERROR <message>`: Error or invalid command.

---

All requests and responses are single lines, newline-terminated. Store files are always saved/loaded in the `/store` directory at the project root.
