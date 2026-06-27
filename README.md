# MiniRedis — In-Memory Key-Value Store in C++

A Redis-inspired key-value store built from scratch in C++.

## Features
- GET, SET, DEL, EXISTS, KEYS, FLUSH, PING over TCP
- LRU eviction using hashmap + doubly linked list
- TTL support — keys auto-expire after set time
- Write-Ahead Log (WAL) — data survives server crashes
- Thread pool — handles multiple concurrent clients
- Benchmark tool — 18K SET and 31K GET ops/second

## Build & Run

```bash
mkdir build && cd build
cmake ..
make

# Terminal 1 — start server
./kv-server 7379

# Terminal 2 — connect client
./kv-client 127.0.0.1 7379

# Terminal 3 — benchmark
./kv-benchmark 1000
```

## Commands
| Command | Example | Description |
|---|---|---|
| SET | SET name Ubhay | Store key-value |
| GET | GET name | Retrieve value |
| DEL | DEL name | Delete key |
| EXISTS | EXISTS name | Check if key exists |
| KEYS | KEYS | List all keys |
| FLUSH | FLUSH | Delete everything |
| PING | PING | Health check |
