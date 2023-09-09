#pragma once

#include <chrono>

#include <userver/logging/log.hpp>

USERVER_NAMESPACE_START

namespace aerospike_nsp {

class Stats {  // rewrite use util::stats
 public:
  Stats(int capacity) {
    ts_errors.reserve(capacity);
    ind = 0;
  }
  void add_error(int error_num) {
    ts_errors[ind] = error_num;
    ind = (ind + 1) % ts_errors.capacity();
  }
  std::vector<std::pair<uint64_t, int>> get_errors() {
    return std::vector<std::pair<uint64_t, int>>{ts_errors.begin() + ind,
                                                 ts_errors.end()} +
           std::vector<std::pair<uint64_t, int>>{ts_errors.begin() + ind,
                                                 ts_errors.begin()};
  }

 private:
  std::vector<std::pair<uint64_t, int>> ts_errors;
  std::chrono::steady_clock clock;
  int ind;
}

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END
