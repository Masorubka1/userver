#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utest/using_namespace_userver.hpp>  // IWYU pragma: keep

#include <string>
#include <string_view>

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/engine/sleep.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include <userver/storages/aerospike/client.hpp>
#include <userver/storages/aerospike/request.hpp>
#include <userver/storages/aerospike/responce.hpp>

#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/utils/from_string.hpp>

namespace chaos {

class KeyValue final : public server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-chaos";

  KeyValue(const components::ComponentConfig& config,
           const components::ComponentContext& context);

  std::string HandleRequestThrow(
      const server::http::HttpRequest& request,
      server::request::RequestContext&) const override;

 private:
  std::string GetValue(std::string_view aerospike_namespace, std::string_view aerospike_set,
                       const server::http::HttpRequest& request) const;
  std::string PostValue(std::string_view aerospike_namespace, std::string_view aerospike_set,
                        const server::http::HttpRequest& request) const;
  std::string DeleteValue(std::string_view aerospike_namespace, std::string_view aerospike_set) const;

  storages::aerospike::Client aerospike_client_;
  //storages::redis::CommandControl aerospike_cc_;
};

KeyValue::KeyValue(const components::ComponentConfig& config,
                   const components::ComponentContext& context)
    : server::handlers::HttpHandlerBase(config, context),
      aerospike_client_{
          context.FindComponent<components::Aerospike>("key-value-database")
              .GetClient("test")} {}
      aerospike_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4} {}

std::string KeyValue::HandleRequestThrow(
    const server::http::HttpRequest& request,
    server::request::RequestContext& /*context*/) const {
  const auto& aerospike_namespace = request.GetArg("namespace");
  if (aerospike_namespace.empty()) {
    throw server::handlers::ClientError(
        server::handlers::ExternalBody{"No 'namespace' query argument"});
  }

  const auto& aerospike_set = request.GetArg("set");
  if (aerospike_set.empty()) {
    throw server::handlers::ClientError(
        server::handlers::ExternalBody{"No 'set' query argument"});
  }

  auto sleep_ms = request.GetArg("sleep_ms");
  if (!sleep_ms.empty()) {
    LOG_DEBUG() << "Sleep for " << sleep_ms << "ms";
    const std::chrono::milliseconds ms{utils::FromString<int>(sleep_ms)};
    engine::SleepFor(ms);
  }

  switch (request.GetMethod()) {
    case server::http::HttpMethod::kGet:
      return GetValue(aerospike_namespace, aerospike_set, request);
    case server::http::HttpMethod::kPost:
      return PostValue(aerospike_namespace, aerospike_set, request);
    case server::http::HttpMethod::kDelete:
      return DeleteValue(aerospike_namespace, aerospike_set);
    default:
      throw server::handlers::ClientError(server::handlers::ExternalBody{
          fmt::format("Unsupported method {}", request.GetMethod())});
  }
}

std::string KeyValue::GetValue(std::string_view key,
                               const server::http::HttpRequest& request) const {
  auto aerospike_request = aerospike_client_->Get(std::string{key}, aerospike_cc_);
  try {
    const auto result = aerospike_request.Get();
    if (!result) {
      request.SetResponseStatus(server::http::HttpStatus::kNotFound);
      return {};
    }
    return *result;
  } catch (const redis::RequestFailedException& e) {
    if (e.GetStatus() == redis::ReplyStatus::kTimeoutError) {
      request.SetResponseStatus(server::http::HttpStatus::kServiceUnavailable);
      return "timeout";
    }

    throw;
  }
}

std::string KeyValue::PostValue(
    std::string_view key, const server::http::HttpRequest& request) const {
  const auto& value = request.GetArg("value");
  auto aerospike_request =
      aerospike_client_->SetIfNotExist(std::string{key}, value, aerospike_cc_);
  try {
    const auto result = aerospike_request.Get();
    if (!result) {
      request.SetResponseStatus(server::http::HttpStatus::kConflict);
      return {};
    }

    request.SetResponseStatus(server::http::HttpStatus::kCreated);
    return std::string{value};
  } catch (const redis::RequestFailedException& e) {
    if (e.GetStatus() == redis::ReplyStatus::kTimeoutError) {
      request.SetResponseStatus(server::http::HttpStatus::kServiceUnavailable);
      return "timeout";
    }

    throw;
  }
}

std::string KeyValue::DeleteValue(std::string_view key) const {
  const auto result = aerospike_client_->Del(std::string{key}, aerospike_cc_).Get();
  return std::to_string(result);
}

}  // namespace chaos

int main(int argc, char* argv[]) {
  const auto component_list =
      components::MinimalServerComponentList()
          .Append<chaos::KeyValue>()
          .Append<components::Aerospike>("key-value-database")
          .Append<components::TestsuiteSupport>();
  return utils::DaemonMain(argc, argv, component_list);
}
