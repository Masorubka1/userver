#pragma once

#include <functional>
#include <map>
#include <memory>
#include <variant>
#include <vector>

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_batch.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_query.h>

#include <aerospike/as_config.h>
#include <aerospike/as_error.h>
#include <aerospike/as_event.h>
#include <aerospike/as_key.h>
#include <aerospike/as_monitor.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_query.h>
#include <aerospike/as_record.h>
#include <aerospike/as_record_iterator.h>

USERVER_NAMESPACE_BEGIN

namespace aerospike_nsp {

namespace {

template <typename T, typename Variant>
constexpr bool IsInVariant = false;

template <typename T, typename... Types>
constexpr bool IsInVariant<T, std::variant<Types...>> =
    (std::is_same_v<T, Types> || ...);

using Aerospike_types = std::variant<aerospike, as_config, as_error, as_monitor,
                                     as_list, as_record, as_key>;

template <typename Type, void (*Destructor)(Type*)>
class Wrapper {
 public:
  Wrapper() {
    static_assert(IsInVariant<Type, Aerospike_types>,
                  "Type is an alternative type in Aerospike_types");
  }

  template <typename... Args>
  explicit Wrapper(Args&&... args) {
    static_assert(IsInVariant<Type, Aerospike_types>,
                  "Type is an alternative type in Aerospike_types");
    Constructor(&val_, std::forward<Args>(args)...);
  }

  /*T& operator*() {
      return &val_;
  }

  const T& operator*() const {
      return &val_;
  }*/

  Type* operator&() { return &val_; }

  const Type* operator&() const { return &val_; }

  Type* operator->() { return &val_; }

  const Type* operator->() const { return &val_; }

  ~Wrapper() { Destructor(&val_); }

 protected:
  Type val_;
};

// In several cases via documentation aerospike we don't want to destroy objects
template <typename T>
void dont_do_anything(T*) {
  return;
}

}  // end anonymous namespace

using Aerospike = Wrapper<aerospike, aerospike_destroy>;
using Error = Wrapper<as_error, dont_do_anything<as_error>>;
using Key = Wrapper<as_key, as_key_destroy>;
using Config = Wrapper<as_config, dont_do_anything<as_config>>;
using Record = Wrapper<as_record, as_record_destroy>;
using Bin = Wrapper<as_bin, dont_do_anything<as_bin>>;

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END