#pragma once

#include <memory>


USERVER_NAMESPACE_BEGIN

namespace engine::impl {
class TaskProcessorPools;
}  // namespace engine::impl

namespace engine::ev {
class ThreadPoolConfig;
} // namespace engine::ev

namespace aerospike_nsp {

class TaskProcessor_Pools {
 public:
  TaskProcessor_Pools(size_t sentinel_thread_pool_size,
                     size_t aerospike_thread_pool_size);
  ~TaskProcessor_Pools();

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