## Redis

The redis asynchronous driver provides an interface to work with Redis
standalone instances, as well as Redis Sentinels and Clusters.

Take note that Redis is not able to guarantee a strong consistency. It is
possible for Redis to lose writes that were acknowledged, and vice
versa.

## Main features

* Convenient methods for Redis commands returning proper C++ types;
* Support for bulk operations (MGET, MSET, etc); driver splits data into smaller
  chunks if necessary to increase server responsiveness;
* Support for different strategies of choosing the most suitable Redis instance;
* Request timeouts management with transparent retries;
* @ref scripts/docs/en/userver/deadline_propagation.md
* Cluster autotopology 

## Metrics

| Metric name           | Description                              |
|-----------------------|------------------------------------------|
| redis.instances_count | current number of Redis instances        |
| redis.last_ping_ms    | last measured ping value                 |
| redis.is_ready        | 1 if connected and ready, 0 otherwise    |
| redis.not_ready_ms    | milliseconds since last ready status     |
| redis.reconnects      | reconnect counter                        |
| redis.session-time-ms | milliseconds since connected to instance |
| redis.timings         | query timings                            |
| redis.errors          | counter of failed requests               |

See @ref scripts/docs/en/userver/service_monitor.md for info on how to get the metrics.

## Usage

To use Redis you must add the component components::Redis and configure it
according to the documentation. After that you can make requests via 
storages::redis::Client:

@snippet storages/redis/client_redistest.cpp Sample Redis Client usage

### Timeouts

Request timeout can be set for a single request via redis::CommandControl 
argument.

Dynamic option @ref REDIS_DEFAULT_COMMAND_CONTROL can be used to set default 
values.

To interrupt a request on the client side, you can use 
@ref task_cancellation_intro "cancellation mechanism" to cancel the task 
that executes the Redis request:

@snippet storages/redis/client_redistest.cpp Sample Redis Cancel request

Redis driver does not guarantee that the cancelled request was not executed
by the server.


### Redis Cluster Autotopology

Cluster autotopology makes it possible to do resharding of the cluster
without Secdist changes and service restart.

With this feature entries in the Secdist are now treated not as a list of all
the cluster hosts, but only as input points through which the service discowers
the configuration of the entire cluster. Therefore, it is not recommended
to delete instances that are listed in secdist from the cluster.

The cluster configuration is checked
* at the start of the service
* and periodically
* and if a MOVED response is received from Redis

If a change in the cluster topology was detected during the check
(hashslot distribution change, master change, or new replicas discowered),
then the internal representation of the topology is recreated,
the new topology is gets ready (new connections may appear in it),
and after that the active topology is replaced.

#### Enabling/Disabling Redis Cluster Autotopology

At the moment aotopology could be nables via an experiment in static config of
a service:

```
#yaml
userver_experiments:
  - redis-cluster-autotopology
```

The autotopology could be disabled by the dynamic config option
REDIS_CLUSTER_AUTOTOPOLOGY_ENABLED_V2.


----------

@htmlonly <div class="bottom-nav"> @endhtmlonly
⇦ @ref scripts/docs/en/userver/mongodb.md | @ref clickhouse_driver ⇨
@htmlonly </div> @endhtmlonly
