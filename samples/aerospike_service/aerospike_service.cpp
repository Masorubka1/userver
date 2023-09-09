#pragma once

#include <iostream>

#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utest/using_namespace_userver.hpp>  // IWYU pragma: keep

#include <userver/storages/aerospike/client.hpp>
#include <userver/storages/aerospike/request.hpp>
#include <userver/storages/aerospike/responce.hpp>
#include <userver/storages/aerospike/thread_pools.hpp>
#include "../../userver/core/src/engine/task/task_processor_config.hpp"

/*namespace userver::engine {

struct TaskProcessorConfig {
  std::string name;

  bool should_guess_cpu_limit{false};
  std::size_t worker_threads{6};
  std::string thread_name;
  OsScheduling os_scheduling{OsScheduling::kNormal};
  int spinning_iterations{10000};

  std::size_t task_trace_every{1000};
  std::size_t task_trace_max_csw{0};
  std::string task_trace_logger_name;

  //void SetName(const std::string& new_name);
};

} // namespace userver::engine*/

/*int main() {
  std::cout << "Start\n";
  auto thread_pools = userver::aerospike_nsp::TaskProcessor_Pools(1, 1);
  userver::engine::TaskProcessorConfig cfg1;
  cfg1.name = "test1";
  cfg1.thread_name = "test1";
  userver::engine::TaskProcessorConfig cfg2;
  cfg2.name = "test2";
  cfg1.thread_name = "test2";
  auto clinet = userver::aerospike_nsp::Client(
      thread_pools.GetSentinelTaskProcessorPools(),
      thread_pools.GetAerospikeTaskProcessorPools(), cfg1, cfg2, "172.20.0.3",
      3000);
  /*auto resp = client.create_request<int, bool>(
      Request<int>{"test", "test", 1},
      userver::aerospike_nsp::AerospikeImpl::Create);
  std::cout << resp.val_.value() << std::endl;
  return 0;
}*/

namespace samples::aerospike_nsp {

}
int main(int argc, char* argv[]) {
  const auto component_list =
      components::MinimalServerComponentList()
          .Append<samples::aerospike_nsp::KeyValue>()
          .Append<samples::aerospike_nsp::EvalSha>()
          .Append<components::Secdist>()
          .Append<components::DefaultSecdistProvider>()
          .Append<components::Aerospike>("key-value-database")
          .Append<components::TestsuiteSupport>()
          .Append<clients::dns::Component>();
  return utils::DaemonMain(argc, argv, component_list);
}