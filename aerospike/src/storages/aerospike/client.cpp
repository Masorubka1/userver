#pragma once

#include <userver/storages/aerospike/client.hpp>

#include <optional>
#include <string>
#include <vector>

#include <userver/engine/task/task_processor_fwd.hpp>
#include <engine/task/task_processor_config.hpp>
#include <engine/task/task_processor_pools.hpp>
#include <engine/task/task_processor.hpp>
#include <userver/engine/task/task.hpp>

#include <userver/storages/aerospike/aerospike_impl.hpp>
#include <userver/storages/aerospike/thread_pools.hpp>

USERVER_NAMESPACE_BEGIN

namespace aerospike_nsp {

Client::Client(
    std::shared_ptr<engine::impl::TaskProcessorPools> sentiel_thread_pool,
    std::shared_ptr<engine::impl::TaskProcessorPools> aerospike_thread_pool,
    const engine::TaskProcessorConfig& sentiel_conf,
    const engine::TaskProcessorConfig& aerospike_conf,
    std::string host = "127.0.0.1", uint16_t port = 3000) {
  /*    engine::TaskProcessor{
          [] {
            engine::TaskProcessorConfig config;
            config.name = "fs-task-processor";
            config.worker_threads = 1;
            config.thread_name = "fs-worker";
            return config;
          }(),
          engine::current_task::GetTaskProcessor().GetTaskProcessorPools()}*/
  sentiel_tasks_ = std::make_unique<engine::TaskProcessor>(sentiel_conf,
                                                           sentiel_thread_pool);

  impl_ = std::make_unique<AerospikeImpl>(
      host, port
      /*std::make_unique<engine::TaskProcessor>(aerospike_conf,
                                              aerospike_thread_pool)*/);
}

template <class T, class U = T>
Responce<U> Client::callback(Request<T> r, AerospikeImpl::Operations o) {
  using Oper = AerospikeImpl::Operations;
  switch (o) {
    case Oper::Create:
      return impl_->key_create(std::move(r));
    case Oper::Set:
      return impl_->key_set(std::move(r));
    case Oper::Operate:
      return impl_->key_operate(std::move(r));
    case Oper::Get:
      return impl_->key_get(std::move(r));
    case Oper::Select:
      return impl_->key_select(std::move(r));
    case Oper::Exists:
      return impl_->key_exists(std::move(r));
    case Oper::Remove:
      return impl_->key_remove(std::move(r));
    default:
      throw std::invalid_argument("Invalid operation");
  }
}

template <class T, class U = T>
Responce<U> Client::callback(BatchRequest<T> r, AerospikeImpl::Operations o) {
  using Oper = AerospikeImpl::Operations;
  switch (o) {
    case Oper::Create:
      return impl_->key_create(std::move(r));
    case Oper::Set:
      return impl_->key_set(std::move(r));
    case Oper::Operate:
      return impl_->key_operate(std::move(r));
    case Oper::Get:
      return impl_->key_get(std::move(r));
    case Oper::Select:
      return impl_->key_select(std::move(r));
    case Oper::Exists:
      return impl_->key_exists(std::move(r));
    case Oper::Remove:
      return impl_->key_remove(std::move(r));
    default:
      throw std::invalid_argument("Invalid operation");
  }
}

template <class T, class U = T>
Responce<U> Client::create_request(Request<T> r, AerospikeImpl::Operations o) {
  return engine::AsyncNoSpan(sentiel_tasks_, [this](Request<T> r, AerospikeImpl::Operations o) {
          return callback<T, U>(std::move(r), o);
      }, std::move(r), o).BlockingWait().Get();
}

template <class T, class U = T>
Responce<U> Client::create_request(BatchRequest<T> r,
                                   AerospikeImpl::Operations o) {
  assert(false);
  return engine::AsyncNoSpan(sentiel_tasks_, [this](BatchRequest<T> r, AerospikeImpl::Operations o) {
          return callback<T, U>(std::move(r), std::move(o));
      }, std::move(r), o).BlockingWait().Get();
}

Client::~Client() {
  // impl_->Disconnect();
  // impl_->ResetRedisObj();
  impl_.reset();
}

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END