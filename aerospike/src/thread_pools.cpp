#include <chrono>
#include <thread>

#include <engine/coro/pool.hpp>
#include <engine/ev/thread_pool.hpp>
#include <engine/ev/thread_pool_config.hpp>
#include <userver/logging/log.hpp>

#include "thread_pools.hpp"

USERVER_NAMESPACE_BEGIN

namespace {

const std::chrono::milliseconds kThreadPoolWaitingSleepTime{20};

const std::string kAerospikeThreadName = "aerospike_client";
const std::string kSentinelThreadName = "aerospike_sentinel";

}  // namespace

namespace aerospike_nsp {

TaskProcessorPools::TaskProcessorPools(size_t sentinel_thread_pool_size,
                                       size_t aerospike_thread_pool_size) {
  sentinel_thread_pool_ = std::make_unique<engine::impl::TaskProcessorPools>(
      engine::coro::PoolConfig{},
      engine::ev::ThreadPoolConfig{sentinel_thread_pool_size,
                                   kSentinelThreadName});

  aerospike_thread_pool_ = std::make_shared<engine::impl::TaskProcessorPools>(
      engine::coro::PoolConfig{},
      engine::ev::ThreadPoolConfig{aerospike_thread_pool_size,
                                   kAerospikeThreadName});
}

TaskProcessorPools::~TaskProcessorPools() {
  LOG_INFO() << "Stopping aerospike thread pools";
  while (aerospike_thread_pool_.use_count() > 1) {
    std::this_thread::sleep_for(kThreadPoolWaitingSleepTime);
  }
  LOG_INFO() << "Stopped aerospike thread pools";
}

std::shared_ptr<engine::impl::TaskProcessorPools>
TaskProcessorPools::GetSentinelTaskProcessorPools() const {
  return sentinel_thread_pool_;
}

std::shared_ptr<engine::impl::TaskProcessorPools>
TaskProcessorPools::GetAerospikeTaskProcessorPools() const {
  return aerospike_thread_pool_;
}

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END