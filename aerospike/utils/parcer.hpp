#pragma once

#include <string>

//#include <aerospike/aerospike.h>

#include <userver/logging/log.hpp>

#include "wrapper_structs.hpp"

USERVER_NAMESPACE_BEGIN

namespace aerospike_nsp {

inline void error(Error& err) {
  LOG_INFO() << "err(" << (int)err->code << ") " << err->message << " at["
             << err->file << ":" << err->line << "]\n";
}

inline std::string dump_bin(const as_bin* p_bin) {
  std::string ans;
  if (!p_bin) {
    LOG_INFO() << "  null as_bin object";
    return "";
  }

  char* val_as_str = as_val_tostring(as_bin_get_value(p_bin));

  LOG_INFO() << " " << as_bin_get_name(p_bin) << ":" << val_as_str;
  ans = std::string(val_as_str);
  free(val_as_str);
  return ans;
}

template <class T>
T get_ans_depends_T(std::vector<std::string> ans) {
  if constexpr (std::is_same<T, int>::value) {
    return std::move(std::stoll(ans[0]));
  } else if constexpr (std::is_same<T, std::string>::value) {
    return std::move(ans[0]);
  } else {
    return std::move(ans);
  }
}

template <class T>
T dump_record(const as_record* p_rec)  // fix me on Record?
{
  std::string key;
  std::string value;
  if (!p_rec) {
    LOG_INFO() << "  null as_record object";
    return T{};
  }

  if (p_rec->key.valuep) {
    char* key_val_as_str = as_val_tostring(p_rec->key.valuep);

    LOG_INFO() << "  key: " << key_val_as_str;
    key = std::move(std::string(key_val_as_str));

    free(key_val_as_str);
  }

  uint16_t num_bins = as_record_numbins(p_rec);

  LOG_INFO() << "  generation " << p_rec->gen << ", ttl " << p_rec->ttl << ", "
             << num_bins << " bin"
             << (num_bins == 0 ? "s" : (num_bins == 1 ? ":" : "s:"));

  as_record_iterator it;
  as_record_iterator_init(&it, p_rec);

  std::vector<std::string> ans;
  while (as_record_iterator_has_next(&it)) {
    ans.emplace_back(dump_bin(as_record_iterator_next(&it)));
  }

  as_record_iterator_destroy(&it);

  return get_ans_depends_T<T>(std::move(ans));
}

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END