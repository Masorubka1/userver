#pragma once

#include <userver/storages/aerospike/thread_pools.hpp>

#include <chrono>
#include <thread>

//#include <engine/coro/pool.hpp>
//#include <engine/ev/thread_pool.hpp>
//#include <engine/ev/thread_pool_config.hpp>
#include <engine/task/task_processor_pools.hpp>

#include <engine/coro/pool_config.hpp>
#include <engine/ev/thread_pool_config.hpp>
#include <engine/task/task_processor_config.hpp>

#include <userver/logging/log.hpp>

USERVER_NAMESPACE_BEGIN

namespace {

const std::chrono::milliseconds kThreadPoolWaitingSleepTime{20};

const std::string kAerospikeThreadName = "aerospike_client";
const std::string kSentinelThreadName = "aerospike_sentinel";

}  // namespace

namespace aerospike_nsp {

TaskProcessor_Pools::TaskProcessor_Pools(size_t sentinel_thread_pool_size,
                                       size_t aerospike_thread_pool_size) {
  auto cfg1 = engine::coro::PoolConfig{};
  auto cfg2 = engine::coro::PoolConfig{};
  auto pool_cfg1 = engine::ev::ThreadPoolConfig{sentinel_thread_pool_size, kSentinelThreadName};
  auto pool_cfg2 = engine::ev::ThreadPoolConfig{aerospike_thread_pool_size, kAerospikeThreadName};
  auto t = engine::impl::TaskProcessorPools(cfg1, pool_cfg1);
  sentinel_thread_pool_ = std::make_unique<engine::impl::TaskProcessorPools>(
      engine::coro::PoolConfig{},
      engine::ev::ThreadPoolConfig{sentinel_thread_pool_size,
                                   kSentinelThreadName});

  aerospike_thread_pool_ = std::make_shared<engine::impl::TaskProcessorPools>(
      engine::coro::PoolConfig{},
      engine::ev::ThreadPoolConfig{aerospike_thread_pool_size,
                                   kAerospikeThreadName});
}

TaskProcessor_Pools::~TaskProcessor_Pools() {
  LOG_INFO() << "Stopping aerospike thread pools";
  while (aerospike_thread_pool_.use_count() > 1) {
    std::this_thread::sleep_for(kThreadPoolWaitingSleepTime);
  }
  LOG_INFO() << "Stopped aerospike thread pools";
}

std::shared_ptr<engine::impl::TaskProcessorPools>
TaskProcessor_Pools::GetSentinelTaskProcessorPools() const {
  return sentinel_thread_pool_;
}

std::shared_ptr<engine::impl::TaskProcessorPools>
TaskProcessor_Pools::GetAerospikeTaskProcessorPools() const {
  return aerospike_thread_pool_;
}

}  // namespace aerospike_nsp

USERVER_NAMESPACE_END