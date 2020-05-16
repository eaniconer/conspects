
# Конкурентность в современных web-service
__Speaker__: _Александр Боргардт_

_link_: https://youtu.be/Ofjx5pv3bo4

## Консольная утилита

- Клиент отправляет на сервер имена файлов
- На сервере есть сервис, который принимает имена файлов. Ищет файлы у себя в хранилище и начинает их конвертировать в некий формат.

```c++
int main()
{
    socket_t s;
    std::string x;
    x = read_from_socket(s, 4);
    if (x == "ping") {
        write_to_socket(s, "pong");
    }

    return 0;
}
```

Можно переписать через колбеки:

```c++
socket_t s;
std::string x;
read_from_socket(s, 4, [s]
{
    // payload
    if (x == "ping") {
        write_to_socket(s, "pong");
    }
});
```

Чтобы эта работало с бОльшим числом клиентов введем сущность `context`

```c++
struct context {
    socket_t socket_;
    std::string data_;
    sub_network_context* buffer_;
};
```

```c++
int main()
{
    std::vector<context> clients;
    std::atomic_bool enabled;

    while (enabled) { // нужен для того, чтобы выключить сервис (может быть глобальной переменной)
        for (auto& client : clients) {
            read_from_socket(client, [&, &client]()
            {
                if (x == "ping") {
                    client.data_ = "pong";
                    write_to_socket(client, [&, &client]()
                    {
                        std::cerr << "Win" << std::endl;
                    });
                }
            });
        }
    }
}
```

```c++
template <class F>
void read_from_socket(context& ctx, F&& callback)
{
    ctx.socket_.read(ctx.buf_raw_, ctx.size_buffer_);
    callback(ctx);
}

template <class F>
void write_to_socket(context& ctx, F&& callback)
{
    ctx.socket_.write(ctx.buff_raw_, ctx.size_buffer_);
    callback(ctx);
}
```

колбеки жестко связываются с read/write, при таком дизайне сложно рефакторить

## Future

```c++
struct request;
struct response;
struct status;

using boost::thread::future;

future<request> read_from_socket(socket_t);
future<response> processing(request);
future<status> write_to_socket(response);
future<socket_t> big_work(socket_t);

int main()
{
    std::vector<socket_t> clients;
    std::atomic_bool enabled;

    while (enabled) {
        for (auto& client : clients) {
            big_work(client) // boost - future позволяют построить пайплайн
                .then(&read_from_socket)
                .then(&processing)
                .then(&write_to_socket);
        }
    }

    return 0;
}
```

Такой код рефакторить проще

#### Boost-future
 * можно указать, чтобы результат future выполняться в другое место: thread-pool, thread
 * разные механики с ожиданиями (time_wait, try_wait, ...)
 * построение пайплайна

## Parallel

```c++
using boost::interprocess::file_mapping;
using boost::interprocess::mapped_region;
using boost::interprocess::read;

struct thread_pool_t {
    void execute(std::function<void(void)>);
};

/* возвращает временные метки, например нарезка 13 минутного фильма вернет
 * {{0, 5}, {5, 10}, {10, 13}}
 */
std::vector<std::pair<float, float>> scissors(boost::string_view);

int main()
{
    file_mapping video_file(path.c_str(), read);
    mapped_region region(video_file, read);
    auto raw = string_view(
        static_cast<char*>(region.get_address()),
        region.get_size());

    auto chunks = scissors(raw);
    std::atomic_size_t count = 0;
    thread_pool_t thread_pool(5);
    for (auto& chunk : chunks) {
        thread_pool.execute([chunk, raw, &count]
        {
            // work
            ++count;
        });
    }
    /// wait: count == chunks.size();

    return 0;
}
```

## Async, Parallel, Concurrent

```c++
std::future<response> video_processing(thread_pool_t& thread_pool, buffer_t buffer)
{
    file_mapping video_file(buffer.c_str(), read);
    mapped_region region(video_file, read);
    auto raw = string_view(
        static_cast<char*>(region.get_address()),
        region.get_size());

    auto chunks = scissors(raw);
    std::atomic_size_t count = chunks.size();
    thread_pool_t thread_pool(5);
    for (auto& chunk : chunks) {
        thread_pool.execute([chunk, raw, &count]
        {
            // work
            --count;
        });
    }

    return // std::future that waits for count == 0;
}

int main()
{
    std::vector<socket_t> clients;
    std::atomic_bool enabled;
    thread_pool_t thread_pool;
    // work
    while (enabled) {
        for (auto& client : clients) {
            big_work(client)
                .then(&read_from_socket)
                .then(&processing)
                .then(bind(video_processing, thread_pool, _1))
                .then(&write_to_socket)
                // detach
                ;
                // work
        }
        // work
    }

    return 0;
}
```

## Проблемы у boost::future

 - трудность построения архитектуры на future (не понятно, что и где исполняется, сколько времени исполняется)
 - высокая степень связности компонентов сервиса
 - неопределенный жизненный цикл у компонентов системы
 - сложность системы склеивания операций в pipeline
 - плохая управляемость памятью
 - при выключении не понятно, кто ответственен за отключение той или иной подсистемы (супервизоры решают эти проблемы)





