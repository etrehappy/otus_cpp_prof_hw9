#pragma once
#include <vector>
#include <string>

class Parser
{
public:
	Parser();
	~Parser();

	void ClearState();
	void Analyses(std::vector<std::string>&);

private:
	void SendCommandsToQueues();

	size_t  start_dynamic_counter_;
	size_t  end_dynamic_counter_;
	const std::string start_dynamic_symbol_;
	const std::string end_dynamic_symbol_;
	std::vector<std::string> commands_;
};
