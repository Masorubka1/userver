#pragma once

#include <engine/coro/pool.hpp>
#include <engine/task/task_processor_pools.hpp>
#include <memory>
#include <userver/engine/task/task_processor_fwd.hpp>

USERVER_NAMESPACE_BEGIN

namespace engine::impl {
class TaskProcessorPools;
}  // namespace engine::impl

namespace aerospike_nsp {

class TaskProcessorPools {
 public:
  TaskProcessorPools(size_t sentinel_thread_pool_size,
                     size_t aerospike_thread_pool_size);
  ~TaskProcessorPools();

  std::shared_ptr<engine::impl::TaskProcessorPools>
  GetSentinelTaskProcessorPools() const;
  std::shared_ptr<engine::impl::TaskProcessorPools>
  GetAerospikeTaskProcessorPools() const;

 private:
  // Sentinel and Aerospike should use separate thread pools to avoid deadlocks.
  // Sentinel waits synchronously while Aerospike starts and stops watchers in
  // Connect()/Disconnect().
  std::shared_ptr<engine::impl::TaskProcessorPools> sentinel_thread_pool_;
  std::shared_ptr<engine::impl::TaskProcessorPools> aerospike_thread_pool_;
};

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END