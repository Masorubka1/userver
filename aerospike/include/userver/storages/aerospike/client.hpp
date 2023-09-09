#pragma once

#include <cassert>

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

#include <userver/engine/task/task.hpp>
#include <userver/engine/task/task_processor_fwd.hpp>
#include <userver/logging/log.hpp>

#include "../utils/parcer.hpp"
#include "../utils/wrapper_structs.hpp"

#include <userver/storages/aerospike/client.hpp>
#include <userver/storages/aerospike/aerospike_impl.hpp>
#include <userver/storages/aerospike/request.hpp>
#include <userver/storages/aerospike/responce.hpp>
#include <userver/storages/aerospike/thread_pools.hpp>

USERVER_NAMESPACE_BEGIN

namespace engine {
class TaskProcessorConfig;
} // namespace engine

/*namespace engine::ev {
class ThreadPool;
class ThreadControl;
}  // namespace engine::ev */

namespace aerospike_nsp {

template <class T>
class Request;

template <class T>
class Responce;

class Client : public std::enable_shared_from_this<Client> {
 public:
  explicit Client(
      std::shared_ptr<engine::impl::TaskProcessorPools> sentiel_thread_pool,
      std::shared_ptr<engine::impl::TaskProcessorPools> aerospike_thread_pool,
      const engine::TaskProcessorConfig& sentiel_conf,
      const engine::TaskProcessorConfig& aerospike_conf, std::string host,
      uint16_t port);

  template <class T, class U = T>
  Responce<U> create_request(Request<T>, AerospikeImpl::Operations o);

  template <class T, class U = T>
  Responce<U> create_request(BatchRequest<T>, AerospikeImpl::Operations o);

  ~Client();

 private:
  template <class T, class U = T>
  Responce<U> callback(Request<T> r, AerospikeImpl::Operations o);

  template <class T, class U = T>
  Responce<U> callback(BatchRequest<T> r, AerospikeImpl::Operations o);

  std::unique_ptr<AerospikeImpl> impl_;
  std::unique_ptr<engine::TaskProcessor> sentiel_tasks_;
};

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END