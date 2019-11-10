__Speaker__: _Alexander Emelin_

### Current state

 - 1.5M requests per minute
 - 2.2M daily unique users
 - 500K simultanious connections (max)
 - 150K notification per minute
 - 7K new messages per minute
 - 2.5K new chats per minute

### Messenger Architecture
_tech stack: golang, kubernetes_
_(see architecture.png)_

_Note: [WebSocket](https://en.wikipedia.org/wiki/WebSocket) - full-duplex communication protocol over single TCP connection_

1. `NGINX`
 - load balancing

2. `service-socker`
 - works inside kubernetes
 - user authentication
 - keeps WebSocket-connection and receive RPC(Remote Procedure Call, _JsonRPC used_) from user
 - sends async-notification from real-time-bus
 - gets RPC from HTTP-fallback (_Except WebSocket, HTTP might be used_)
 - trottling RPC (token bucket) - to get rid of _spurious traffic_ caused by client-apps' bugs or "very smart" users ([Read here](https://stripe.com/blog/rate-limiters))
 - convert PRC to HTTP

3. `service-aggregator`
 - business logic
 - build responses to client-apps with respect to contracts
 - `graceful degradation` in case lack of access to some services
 - if a client sends message, `service-aggregator`redirect message to `service-db-api`

4. `service-db-api`
 - wrapper over main database (connect to MongoDB over TCP)
 - saves data to sender-shard
 - publishes events to queue `RabbitMQ`

5. `MongoDB`
 - 8 separate independent replicas (client-sharding & virtual buckets used to distribute data by users and by shards. sharding by user-id)
 - 1 message is saved to 2 shards (1 shard for each user). It is _flexible_, but _redundant_.

6. `RabbitMQ`
 - fault tolerant cluster consiting of two computers
 - fault tolerant policy for exchange points and queues
 - retry system: RabbitMQ DLX+TTL

7. `service-db-store`
 - reads events from `RabbitMQ` and process them asynchronously
 - saves data to receiver-shard
 - publish message to real-time-bus of notification
 - retry logic, _in case something goes wrong_ (Idempotent operation: check ids of saved message entry of published notification to avoid doing this twice or more times)

8. `service-middleware`
 - reads message in `RabbitMQ` before them go to `service-db-store`
 - antispam checking with graceful degradation (time of checking is restricted)

9. `real-time-notification bus` / `SockStream Redis`
 - `service-db-store` published notification, `service-socket` subscribed on notification
 - instant notification
 - stores warm(hot) message cache during retention-period (for recconection, HTTP-fallbacks(_each 10 secs send info to users who used HTTP_))
 - supports a lot of active subscriptions on various user topics

 - See `Centrifugo` (_the speaker's project_) - real-time notification server













