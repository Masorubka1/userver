#pragma once

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_batch.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_query.h>
#include <aerospike/as_config.h>
#include <aerospike/as_event.h>
#include <aerospike/as_key.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_query.h>
#include <aerospike/as_record.h>
#include <aerospike/as_record_iterator.h>

#include <userver/engine/async.hpp>
#include <userver/engine/task/task.hpp>
#include <userver/engine/task/task_processor_fwd.hpp>
//#include <userver/engine/task/task_processor.hpp>
#include <userver/logging/log.hpp>

#include "../utils/parcer.hpp"
#include "../utils/wrapper_structs.hpp"

#include <userver/storages/aerospike/request.hpp>
#include <userver/storages/aerospike/responce.hpp>

USERVER_NAMESPACE_BEGIN

namespace aerospike_nsp {

template <class T>
class Request;

template <class T>
class Responce;

inline void handle_error(Error& err) {
  LOG_INFO() << "err(" << (int)err->code << ") " << err->message << " at["
             << err->file << ":" << err->line << "]\n";
}

class AerospikeImpl {
 public:
  enum Operations : uint8_t {
    Create,
    Set,
    Operate,
    Get,
    Select,
    Exists,
    Remove,
  };

  explicit AerospikeImpl(std::string host, uint16_t port
                         /*std::unique_ptr<engine::TaskProcessor> task_processor*/)
      /*: tasks_(std::move(task_processor))*/ {
    as_config_init(&config_);
    as_config_add_host(&config_, host.c_str(), port);

    aerospike_init(&client_, &config_);
  }

  Error connect() {
    Error err;
    if (aerospike_close(&client_, &err) != AEROSPIKE_OK) {
      error(err);
    }
    return err;
  }

  const std::vector<const char*> get_bins(
      std::optional<std::vector<std::string>> bins) {
    std::vector<const char*> ans;
    if (!bins.has_value()) {
      return ans;
    }

    for (const auto& i : bins.value()) {
      ans.emplace_back(i.c_str());
    }
    return ans;
  }

  template <class T>
  Responce<bool> key_create(Request<T> r) {
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Responce<bool> rsp(std::move(&r.key), true);
                 if (aerospike_key_put(&client_, &err, NULL, &rsp.key, NULL) !=
                     AEROSPIKE_OK) {
                   handle_error(err);
                   rsp.set_value(false);
                 }
                 return std::move(rsp);
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  template <class T>
  Responce<bool> key_set(Request<T> r) {
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Record rec;
                 as_record_set_str(&rec, r.key, r.vals);
                 Responce<bool> rsp(std::move(&r.key), true);
                 if (aerospike_key_put(&client_, &err, NULL, &r.key, rec) !=
                     AEROSPIKE_OK) {
                   handle_error(err);
                   rsp.set_value(false);
                 }
                 return std::move(rsp);
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  template <class T>
  Responce<T> key_operate(Request<T> r) {  //?
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Record rec;
                 as_record_set_str(&rec, r.key, r.vals);
                 Responce<bool> rsp(std::move(&r.key), true);
                 if (aerospike_key_put(&client_, &err, NULL, &r.key, rec) !=
                     AEROSPIKE_OK) {
                   handle_error(err);
                   rsp.set_value(false);
                 }
                 return std::move(rsp);
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  template <class T>
  Responce<std::string> key_get(Request<T> r) {
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Record rec;
                 Responce<std::string> rsp(std::move(&r.key), "");
                 if (aerospike_key_get(&client_, &err, NULL, &r.key, &rec) !=
                     AEROSPIKE_OK) {
                   handle_error(err);
                   return rsp;
                 }
                 rsp.set_value(dump_record<T>(&rec));
                 return std::move(rsp);
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  template <class T>
  Responce<std::map<std::string, T>> key_select(Request<T> r) {
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Record rec;
                 // static const char* bins_1_3[] = { "test-bin-1",
                 // "test-bin-3", NULL }; // support bins
                 std::vector<const char*> bins = get_bins(r.bins_);
                 std::string ans;
                 if (aerospike_key_select(&client_, &err, NULL, &r.key,
                                          bins.size() > 0 ? &bins[0] : NULL,
                                          &rec) == AEROSPIKE_OK) {
                   ans = dump_record<T>(&rec);
                 } else {
                   handle_error(err);
                 }
                 return ans;
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  template <class T>
  Responce<bool> key_exists(Request<T> r) {
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Record rec;
                 Responce<bool> rsp(std::move(&r.key), true);
                 if (aerospike_key_exists(&client_, &err, NULL, &r.key, &rec) ==
                     AEROSPIKE_ERR_RECORD_NOT_FOUND) {
                   handle_error(err);
                   rsp.set_value(false);
                 }
                 return std::move(rsp);
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  /*template <class T>
  Responce<T> key_operate(Request<T> r) { //?
      return engine::AsyncNoSpan(engine::current_task::GetTaskProcessor(),
  [this](Request<T> r) { Error err = std::move(connect()); Record rec;
          std::string ans;
          if (aerospike_key_select(&client_, &err, NULL, &r.key, &rec) ==
  AEROSPIKE_OK) { ans = dump_record(&rec); } else { handle_error(err);
          }
          return ans;
      }, r).Get();
  }*/

  template <class T>
  Responce<bool> key_remove(Request<T> r) {
    return engine::AsyncNoSpan(
               /*tasks_.get()*/
               engine::current_task::GetTaskProcessor(),
               [this](Request<T> r) {
                 Error err = std::move(connect());
                 Responce<bool> rsp(std::move(&r.key), true);
                 if (aerospike_key_remove(&client_, &err, NULL, &r.key) !=
                     AEROSPIKE_OK) {
                   handle_error(err);
                   rsp.set_value(false);
                 }
                 return std::move(rsp);
               },
               std::move(r))
        .BlockingWait()
        .Get();
  }

  ~AerospikeImpl() {
    delete &client_;
    delete &config_;
    // delete &tasks_;
  }

 private:
  // void error_handler();
  // void establish_connection();

  Aerospike client_;
  Config config_;
  //std::unique_ptr<engine::TaskProcessor> tasks_;
};

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END