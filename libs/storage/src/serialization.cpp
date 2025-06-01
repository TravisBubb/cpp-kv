#include "../include/serialization.h"

void write_bytes(std::ostream &f, const std::string &s) {
  std::size_t size = s.size();
  f.write(reinterpret_cast<const char *>(&size), sizeof(size));
  f.write(s.data(), size);
}

void write_bytes(std::ostream &f, const std::vector<uint8_t> &vec) {
  std::size_t size = vec.size();
  f.write(reinterpret_cast<const char *>(&size), sizeof(size));
  f.write(reinterpret_cast<const char *>(vec.data()), size);
}

void read_bytes(std::istream &f, std::string &s) {
  std::size_t size;
  f.read(reinterpret_cast<char *>(&size), sizeof(size));

  s.resize(size);
  f.read(s.data(), size);
}

void read_bytes(std::istream &f, std::vector<uint8_t> &vec) {
  std::size_t size;
  f.read(reinterpret_cast<char *>(&size), sizeof(size));

  vec.resize(size);
  f.read(reinterpret_cast<char *>(vec.data()), size);
}
