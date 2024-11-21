#include "parser.h"
#include "safe_queue.h"

namespace async
{
    extern ThreadSafeQueue kQueue;
}


Parser::Parser()
    :start_dynamic_counter_{0}, end_dynamic_counter_{0}, 
    start_dynamic_symbol_{"{"}, end_dynamic_symbol_{"}"},
    commands_{}
{  
}

Parser::~Parser()
{

}

void Parser::ClearState()
{
    start_dynamic_counter_ = 0;
    end_dynamic_counter_ = 0;
    commands_.clear();
}

void Parser::Analyses(std::vector<std::string>& strings)
{        

    for (auto& string : strings)
    {        
        if (string == start_dynamic_symbol_)
        {  
            ++start_dynamic_counter_;

            if (!commands_.empty() && start_dynamic_counter_ == 1)
            {
                SendCommandsToQueues();
            }
                     
            continue;
        }

        if (string == end_dynamic_symbol_)
        {            
            if (start_dynamic_counter_ == 0) { continue; }

            if (++end_dynamic_counter_ == start_dynamic_counter_)
            {
                start_dynamic_counter_ = 0;
                end_dynamic_counter_ = 0;

                if (!commands_.empty())
                {
                    SendCommandsToQueues();  
                }            
            }
            continue;
        }     

        commands_.push_back(std::move(string));       

    }

    if (!commands_.empty() && start_dynamic_counter_ == 0)
    {
        SendCommandsToQueues();
    }

}


void Parser::SendCommandsToQueues()
{
    async::kQueue.MovePush(commands_);

    commands_.clear();
}

