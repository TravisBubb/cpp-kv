## Goal
Build a distributed key-value store with basic replication, cluster membership, and leader-based write coordination.

## High-Level Roadmap

### 🟢 Phase 1: Core MVP

> Goal: Show distributed coordination and replication working — basic, but real.

#### Core Features to Implement:

|Feature|Description|
|---|---|
|In-memory store|Use `unordered_map<string, string>` for now|
|Simple WAL |Append-only log for persistence|
|`KVServer`|Accepts client requests (GET, PUT, DELETE) over TCP|
|`ClusterNode`|Each node can act as leader or follower|
|`ClusterManager`|Static config for cluster bootstrapping|
|Leader routing|Clients send writes to the leader; followers forward them|
|Log replication|Leader sends write ops to followers (basic 2-phase apply)|
|Client CLI|Tool to send requests to any node (reads/writes/health)|

---

### 🟡 Phase 2: More Advanced Distributed Features & Persistence (DRAFT)

> Goal: Start to turn CPP-KV into a real-world system. 

#### Ideas to play with, research, and design:

|Topic|Notes|
|---|---|
|Consensus|Raft-style leader election, log replication, commit index|
|Consistency|Explain tradeoffs (e.g. leader-based strong consistency vs. quorum-based eventual consistency)|
|Persistence|Memtable + SSTable + WAL design (LSM-tree style)|
|Sharding|Consistent hashing and virtual nodes|
|Failover|Follower becomes leader if heartbeat fails|

---

### 🔵 Phase 3: Stretch Goals (DRAFT)

> Goal: Extra nice-to-have features that improve overall usability.

#### Ideas:

|Feature|Description|
|---|---|
|Heartbeats|Periodic pings from leader to followers|
|Compaction|Merge and cleanup old logs/snapshots|
|Dynamic config|Add/remove nodes without full restart|
|Snapshotting|Periodic state snapshots for log truncation|
|Metrics|Simple metrics endpoint or log output|
|Monitoring CLI/UI|Simple console for viewing node status|

