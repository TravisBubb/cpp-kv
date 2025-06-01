#include "../include/serialization.h"
#include "../include/wal.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;

const std::string WAL_FILE = "testwal.bin";

WalEntry read_entry(std::ifstream &f);
std::vector<WalEntry> read_all_entries(const std::string &path);
void delete_wal_file();
void initialize_wal_file(const std::vector<WalEntry> &entries);

class WalManagerTest : public ::testing::Test {
protected:
  void SetUp() override { delete_wal_file(); }

  void TearDown() override { delete_wal_file(); }
};

TEST_F(WalManagerTest, CanWriteSetEntry) {
  WalManager wal;
  std::string key = "foo";
  std::vector<uint8_t> data = {'b', 'a', 'r'};

  auto future = wal.set(key, data);
  ASSERT_EQ(future.get(), WalResult::OK);

  auto entries = read_all_entries(WAL_FILE);
  ASSERT_EQ(entries.size(), 1);
  EXPECT_EQ(entries[0].type, WalCommandType::Set);
  EXPECT_EQ(entries[0].key, key);
  EXPECT_EQ(entries[0].data, data);
}

TEST_F(WalManagerTest, CanWriteRemoveEntry) {
  WalManager wal;
  std::string key = "delete_me";

  auto future = wal.remove(key);
  ASSERT_EQ(future.get(), WalResult::OK);

  auto entries = read_all_entries(WAL_FILE);
  ASSERT_EQ(entries.size(), 1);
  EXPECT_EQ(entries[0].type, WalCommandType::Remove);
  EXPECT_EQ(entries[0].key, key);
  EXPECT_TRUE(entries[0].data.empty());
}

TEST_F(WalManagerTest, HandlesMultipleOperations) {
  WalManager wal;

  auto fut1 = wal.set("a", {'1'});
  auto fut2 = wal.remove("b");
  auto fut3 = wal.set("c", {'3'});

  EXPECT_EQ(fut1.get(), WalResult::OK);
  EXPECT_EQ(fut2.get(), WalResult::OK);
  EXPECT_EQ(fut3.get(), WalResult::OK);

  auto entries = read_all_entries(WAL_FILE);
  ASSERT_EQ(entries.size(), 3);

  EXPECT_EQ(entries[0].type, WalCommandType::Set);
  EXPECT_EQ(entries[1].type, WalCommandType::Remove);
  EXPECT_EQ(entries[2].type, WalCommandType::Set);
}

TEST_F(WalManagerTest, HandlesEmptyKeyAndData) {
  WalManager wal;

  auto fut1 = wal.set("", {});
  auto fut2 = wal.remove("");

  EXPECT_EQ(fut1.get(), WalResult::OK);
  EXPECT_EQ(fut2.get(), WalResult::OK);

  auto entries = read_all_entries(WAL_FILE);
  ASSERT_EQ(entries.size(), 2);

  EXPECT_EQ(entries[0].key, "");
  EXPECT_TRUE(entries[0].data.empty());

  EXPECT_EQ(entries[1].key, "");
  EXPECT_TRUE(entries[1].data.empty());
}

TEST_F(WalManagerTest, HandlesLargeData) {
  WalManager wal;

  std::vector<uint8_t> big_data(1024 * 1024, 42); // 1 MB of 0x2A
  auto fut = wal.set("bigkey", big_data);

  EXPECT_EQ(fut.get(), WalResult::OK);

  auto entries = read_all_entries(WAL_FILE);
  ASSERT_EQ(entries.size(), 1);
  EXPECT_EQ(entries[0].key, "bigkey");
  EXPECT_EQ(entries[0].data, big_data);
}

TEST_F(WalManagerTest, CanStreamEntries) {
  WalManager wal;

  std::vector<WalEntry> original_entries = {
      {WalCommandType::Set, "key1", {'v', 'a', 'l', '1'}},
      {WalCommandType::Set, "key2", {'v', 'a', 'l', '2'}},
      {WalCommandType::Set, "key3", {'v', 'a', 'l', '3'}}};

  initialize_wal_file(original_entries);

  std::vector<WalEntry> streamed_entries;
  wal.stream_entries(
      [&](const WalEntry &entry) { streamed_entries.push_back(entry); });

  ASSERT_EQ(streamed_entries.size(), 3);
  for (size_t i = 0; i < original_entries.size(); ++i) {
    EXPECT_EQ(streamed_entries[i].type, original_entries[i].type);
    EXPECT_EQ(streamed_entries[i].key, original_entries[i].key);
    EXPECT_EQ(streamed_entries[i].data, original_entries[i].data);
  }
}

WalEntry read_entry(std::ifstream &f) {
  WalEntry e;
  read_bytes(f, e.type);
  read_bytes(f, e.key);
  read_bytes(f, e.data);
  return e;
}

std::vector<WalEntry> read_all_entries(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  std::vector<WalEntry> entries;

  while (f.peek() != EOF) {
    WalEntry e = read_entry(f);
    entries.push_back(std::move(e));
  }

  return entries;
}

void delete_wal_file() {
  if (fs::exists(WAL_FILE)) {
    fs::remove(WAL_FILE);
  }
}

void initialize_wal_file(const std::vector<WalEntry> &entries) {
  std::ofstream f(WAL_FILE, std::ios::binary);
  ASSERT_TRUE(f.is_open());

  for (const auto &entry : entries) {
    write_bytes(f, entry.type);
    write_bytes(f, entry.key);
    write_bytes(f, entry.data);
  }

  f.close();
}
