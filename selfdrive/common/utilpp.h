#pragma once

#include <cstdio>
#include <unistd.h>

#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <thread>
#include <chrono>

namespace util {

inline bool starts_with(std::string s, std::string prefix) {
  return s.compare(0, prefix.size(), prefix) == 0;
}

template<typename ... Args>
inline std::string string_format( const std::string& format, Args ... args ) {
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 );
}

inline std::string read_file(std::string fn) {
  std::ifstream t(fn);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

inline std::string tohex(const uint8_t* buf, size_t buf_size) {
  std::unique_ptr<char[]> hexbuf(new char[buf_size*2+1]);
  for (size_t i=0; i < buf_size; i++) {
    sprintf(&hexbuf[i*2], "%02x", buf[i]);
  }
  hexbuf[buf_size*2] = 0;
  return std::string(hexbuf.get(), hexbuf.get() + buf_size*2);
}

inline std::string base_name(std::string const & path) {
  size_t pos = path.find_last_of("/");
  if (pos == std::string::npos) return path;
  return path.substr(pos + 1);
}

inline std::string dir_name(std::string const & path) {
  size_t pos = path.find_last_of("/");
  if (pos == std::string::npos) return "";
  return path.substr(0, pos);
}

inline std::string readlink(std::string path) {
  char buff[4096];
  ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
  if (len != -1) {
    buff[len] = '\0';
    return std::string(buff);
  }
  return "";
}

inline std::string getenv_default(const char* env_var, const char * suffix, const char* default_val) {
  const char* env_val = getenv(env_var);
  if (env_val != NULL){
    return std::string(env_val) + std::string(suffix);
  } else {
    return std::string(default_val);
  }
}

inline void sleep_for(const int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
}

struct unique_fd {
  unique_fd(int fd = -1) : fd_(fd) {}
  unique_fd& operator=(unique_fd&& uf) {
    fd_ = uf.fd_;
    uf.fd_ = -1;
    return *this;
  }
  ~unique_fd() {
    if (fd_ != -1) close(fd_);
  }
  operator int() const { return fd_; }
  int fd_;
};
