#pragma once

#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <thread>
#include <sstream>
#include "async.h"
#include "cmd_reader.h"
#include "parser.h"
#include "safe_queue.h"

class Context
{
public:
    Context(std::size_t bulk_size);

    void ProcessData(std::istream& input);

    std::mutex context_mutex_;

private:

    std::size_t bulk_size_;
    Parser parser_;
    CommandReader reader_;
};

class Controller
{
public:
    static Controller& Instance() { static Controller instance; return instance; }

    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;
    void StopThreads();
    std::unordered_map<async::handle_t, std::shared_ptr<Context>>& GetContextList();

    async::handle_t GetNewConnectHandle(size_t bulk)
    {
        auto temp_context = std::make_shared<Context>(bulk);
        async::handle_t handle = reinterpret_cast<async::handle_t>(temp_context.get());
        context_container_[handle] = std::move(temp_context);
        return handle;
    }

private:
    Controller();
    ~Controller();

    void PrintToConsole();
    void PrintToFile();

    bool stop_threads_{false};
    std::thread console_thread_{}, file_thread_1_{}, file_thread_2_{};

    std::unordered_map<async::handle_t, std::shared_ptr<Context>> context_container_{};

};


