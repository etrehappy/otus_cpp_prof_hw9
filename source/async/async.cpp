
#include "async.h"
#include "controller.h"

namespace async
{
    extern Controller* kController;
} // namespace async

namespace async
{

    handle_t connect(std::size_t bulk)
    {
        if (!async::kController)
        {
            Controller& сontroller = Controller::Instance();
            kController = &сontroller;
        }
        
        
        return kController->GetNewConnectHandle(bulk);
    }

    void receive(handle_t handle, const char* data, std::size_t size)
    {       
        auto it = kController->GetContextList().find(handle);

        if (it != kController->GetContextList().end())
        {
            std::istringstream stream(std::string{data, size});

            Context& context = *it->second;
            std::lock_guard<std::mutex> lock(context.context_mutex_);

            context.ProcessData(stream);
        }
    }

    void disconnect(handle_t handle)
    {
         auto it = kController->GetContextList().find(handle);

        if (it != kController->GetContextList().end())
        {                       
            kController->GetContextList().erase(handle);                       
        }
              
        if (kController->GetContextList().empty())
        {
            kController->StopThreads();
        }
    }

} // namespace async 

 