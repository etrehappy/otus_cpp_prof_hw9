#include "controller.h"
#include <filesystem>
#include <iostream>

#include <fstream>

namespace async
{

    static std::mutex kCoutMutex{}, kFileOutMutex{}, kQueueMutex{};
    static const char* kDirPath{"./logs/"};

    std::condition_variable kConsoleCv{}, kFileCv{};
    ThreadSafeQueue kQueue{};
    Controller* kController{nullptr};
    static std::ostream& cout = std::cout;
} // namespace async 

Controller::Controller()
{
    if (!std::filesystem::exists(async::kDirPath))
    {
        std::filesystem::create_directories(async::kDirPath);
    }

    console_thread_ = std::thread(&Controller::PrintToConsole, this);
    file_thread_1_ = std::thread(&Controller::PrintToFile, this);
    file_thread_2_ = std::thread(&Controller::PrintToFile, this);
}

Controller::~Controller()
{
    StopThreads();
}

void Controller::StopThreads()
{
    {
        std::unique_lock<std::mutex> lock(async::kCoutMutex);
        stop_threads_ = true;
    }

    if (console_thread_.joinable()) { console_thread_.join(); }
    if (file_thread_1_.joinable()) { file_thread_1_.join(); }
    if (file_thread_2_.joinable()) { file_thread_2_.join(); }
}

std::unordered_map<async::handle_t, std::shared_ptr<Context>>& Controller::GetContextList()
{
    return context_container_;
}

void Controller::PrintToConsole()
{
    std::unique_lock<std::mutex> lock(async::kCoutMutex);


    while (true)
    {
        /* 1. Проверка */

        if (async::kQueue.IsEmpty()) {
            if (stop_threads_) return;
            async::kConsoleCv.wait(lock, [this] { return !async::kQueue.IsEmpty() || stop_threads_; });
        }

        if (stop_threads_ && async::kQueue.IsEmpty()) return;

        QueueItem* item = async::kQueue.GetFront();

        if (item == nullptr || item->console_ready_) { continue; }

        /* 2. Вывод */
        async::cout << "bulk: ";

        if (item->data_.size() > 1)
        {
            for (auto it = item->data_.cbegin(); it != item->data_.cend() - 1; it++)
            {
                async::cout << *it << ", ";
            }
        }

        async::cout << item->data_.back() << std::endl;

        /* 3. Удаление */

        if (item->log_ready_)
        {
            async::kQueue.Pop();
            continue;
        }
        item->console_ready_ = true;

    }
}

static int kFileCounter = 0;

void Controller::PrintToFile()
{
    std::unique_lock<std::mutex> lock(async::kFileOutMutex);


    while (true)
    {
        /* 1. Проверка */

        if (async::kQueue.IsEmpty()) {
            if (stop_threads_) return;
            async::kFileCv.wait(lock, [this] { return !async::kQueue.IsEmpty() || stop_threads_; });
        }
        if (stop_threads_ && async::kQueue.IsEmpty()) return;


        QueueItem* item = async::kQueue.GetFront();

        if (item == nullptr || item->log_ready_) { continue; }

        /* 2. Подготовка */

        std::ostringstream oss_t_id;
        oss_t_id << std::this_thread::get_id();
        const std::string& thread_id = oss_t_id.str();

        std::string time = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

        std::ofstream out_file(std::string{async::kDirPath} + "bulk_" + thread_id + "_" + time + std::to_string(++kFileCounter) + ".log", std::ios::app);

        /* 3. Вывод */

        if (item->data_.size() > 1)
        {
            for (auto it = item->data_.cbegin(); it != item->data_.cend() - 1; it++)
            {
                out_file << *it << ", ";
            }
        }
        if (item->data_.back() != "\n")
        {
            out_file << item->data_.back();
        }


        /* 4. Удаление */

        if (item->console_ready_)
        {
            async::kQueue.Pop();
            continue;
        }
        item->log_ready_ = true;

    }

}

Context::Context(std::size_t s)
    :bulk_size_{s}, parser_{}, reader_{s}
{
}

void Context::ProcessData(std::istream& input)
{
    parser_.ClearState();
    reader_.ClearState();

    while (!reader_.IsEof())
    {
        reader_.Read(input);
        parser_.Analyses(reader_.GetCommands());
    }
}
