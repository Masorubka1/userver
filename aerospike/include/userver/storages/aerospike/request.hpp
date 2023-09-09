#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include <userver/engine/deadline.hpp>
#include <userver/engine/future.hpp>

#include "../utils/wrapper_structs.hpp"
#include <userver/storages/aerospike/responce.hpp>

USERVER_NAMESPACE_BEGIN

namespace aerospike_nsp {

/*template <typename T>
concept VectorStringIntOrNullopt = std::is_same_v<T, std::vector<typename
T::value_type>> || std::is_same_v<T, std::string> || std::is_same_v<T, int> ||
                                    std::is_same_v<T, std::nullopt_t>;*/

using Request_types = std::variant<bool, int, std::string, std::vector<int>,
                                   std::vector<std::string>>;

template <class T>
class Request {
 public:
  explicit Request(std::string& str_namespace, std::string& str_set, T val,
                   std::optional<std::vector<std::string>> bins) {
    static_assert(IsInVariant<T, Request_types>,
                  "Type is an alternative type in Request_types");
    as_key_init_str(&key, str_namespace, str_set, val);
    vals = val;
    bins_ = bins;
  }
  Key key;
  std::optional<T> vals;
  std::optional<std::vector<std::string>> bins_;

  // Responce<T> Get() = delete;
 private:
  // engine::Future<Responce<T>> future_;
  // engine::Deadline deadline_;
};

template <class T>
class BatchRequest {
 public:
  explicit BatchRequest(std::vector<aerospike_nsp::Request<T>> requests) {
    req_ = requests;
  }
  std::vector<Request<T>> req_;
};

// client.Get({"ns", "key", val});
//  отдельные классы реквестов для разных типов
//  batch отдельный request/responce
//
}  // namespace aerospike_nsp

USERVER_NAMESPACE_END