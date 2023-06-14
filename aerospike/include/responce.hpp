#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

USERVER_NAMESPACE_BEGIN

namespace aerospike_nsp {

/*template <typename T>
concept VectorStringIntOrNullopt = std::is_same_v<T, std::vector<typename
T::value_type>> || std::is_same_v<T, std::string> || std::is_same_v<T, int> ||
                                    std::is_same_v<T, std::nullopt_t>;*/

// constraints on template T same as in Request.hpp

template <class T>
class Responce {
  explicit Responce(Key key, std::optional<T> val = std::nullopt) {
    key = key;
    val_ = val;
  }
  void set_value(T val) { val_ = val; }
  // private:
  Key key;
  std::optional<T> val_;

  class Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = value_type&;
    using pointer = value_type*;

    explicit Iterator(Responce* stream) : stream_(stream) {
      if (stream_ && !stream_->HasMore()) stream_ = nullptr;
    }

    class ReplyElemHolder {
     public:
      ReplyElemHolder(value_type reply_elem)
          : reply_elem_(std::move(reply_elem)) {}

      value_type& operator*() { return reply_elem_; }

     private:
      value_type reply_elem_;
    };

    ReplyElemHolder operator++(int) {
      ReplyElemHolder old_value(stream_->Current());
      ++*this;
      return old_value;
    }

    Iterator& operator++() {
      stream_->Get();
      if (!stream_->HasMore()) stream_ = nullptr;
      return *this;
    }

    reference operator*() { return stream_->Current(); }

    pointer operator->() { return &**this; }

    bool operator==(const Iterator& rhs) const {
      return stream_ == rhs.stream_;
    }

    bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

   private:
    Responce* stream_;
  };

 private:
  bool HasMore() {
    if constexpr (std::is_same_v<
                      std::decay_t<T>,
                      std::vector<typename std::decay_t<T>::value_type> >) {
      return val_.size() != val_.capacity();
    } else {
      return false;
    }
  }
  ~Responce() {}
};

// client.Get({"ns", "key", val});
//  отдельные классы реквестов для разных типов
//  batch отдельный request/responce
//
}  // namespace aerospike_nsp

USERVER_NAMESPACE_END