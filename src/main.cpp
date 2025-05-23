#include "../include/in_memory_engine.h"
#include <cstdio>

int main() {
  auto engine = InMemoryStorageEngine();
  std::string v1 = "v1";
  std::printf("Setting k1:v1\n");
  engine.set("k1", std::vector<uint8_t>(v1.begin(), v1.end()));
  StorageResult res = engine.get("k1");
  if (res.get_status() == StorageStatus::OK) {
    if (!res.get_data().has_value()) {
      std::printf("No value found for k1\n");
      return 1;
    }
    std::vector<std::uint8_t> bytes = res.get_data().value();
    std::string str = std::string(bytes.begin(), bytes.end());
    std::printf("Retrieved k1:%s\n", str.c_str());
  } else {
    std::printf("Received error status\n");
  }
  return 0;
}
