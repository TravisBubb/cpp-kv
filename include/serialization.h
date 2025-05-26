#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <cstdint>
#include <istream>
#include <ostream>
#include <type_traits>
#include <vector>

template <typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value>::type
write_bytes(std::ostream &f, const T &t) {
  f.write(reinterpret_cast<const char *>(&t), sizeof(T));
}

void write_bytes(std::ostream &f, const std::string &s);

void write_bytes(std::ostream &f, const std::vector<uint8_t> &vec);

template <typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value, void>::type
read_bytes(std::istream &f, T &t) {
  f.read(reinterpret_cast<char *>(&t), sizeof(T));
}

void read_bytes(std::istream &f, std::string &s);

void read_bytes(std::istream &f, std::vector<uint8_t> &vec);

#endif
