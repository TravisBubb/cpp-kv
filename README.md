# Distributed Key-Value Store (C++)

[![CI](https://github.com/TravisBubb/cpp-kv/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/TravisBubb/cpp-kv/actions/workflows/unit_tests.yml)

A high-performance, distributed key-value store written in C++. This project is designed as both a practical storage system and a learning exploration of the architecture of distributed storage systems.

---

## 🚀 Features

### MVP
- [x] In-memory key-value storage (string keys, binary data)
- [x] Write-Ahead Log (WAL) for simple persistence and crash recovery
- [ ] Coordinator node with REST API
- [ ] Basic cluster discovery mechanism
- [ ] Leader-follower replication

### Future
- [ ] Memtable + SSTable-based persistence layer
- [ ] WAL compaction
- [ ] Data time-to-live (TTL)
- [ ] Cluster bootstraping and dynamic discovery
- [ ] Leader election and routing logic
- [ ] Cluster metrics and monitoring

---

## 🧱 Architecture Overview

The MVP system will consist of three primary components:

- **Coordinator**: Receives client requests and routes them to the appropriate
  partition/shard leader. 
- **KV Nodes**: Handle the read/write operations; can be leaders or followers
  within a certain partition.
- **WAL (Write-Ahead Log)**: Ensures data durability and crash recovery before
  writes are applied to memory.

> 📊 Detailed architecture diagrams and design documents available in the [`/docs`](./docs) folder (WIP).

---

## 🔧 Building

### Requirements
- C++20 or newer
- CMake 3.16+
- A modern compiler (GCC, Clang, MSVC)
- `make` or equivalent build tool

### Build Steps
```bash
git clone https://github.com/TravisBubb/cpp-kv.git
cd cpp-kv
mkdir build && cd build
cmake ..
make
```

---

## 🤝 Contributing

While this project is primarily for personal/educational use, contributions,
feedback, and code reviews are always welcome. Feel free to open issues or PRs!
