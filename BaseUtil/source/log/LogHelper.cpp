#include "log/LogHelper.h"
#include <spdlog/spdlog.h>

namespace BaseUtil
{
	std::shared_ptr<LogHelper> GetLogHelper(int level)
	{
		return std::make_shared<LogHelper>((spdlog::level::level_enum)level);
	}

	LogHelper::LogHelper(int level)
		:level_(level)
	{
	}

	void LogHelper::log()
	{
		GetLogger().log(level_, string_stream_.str());
		string_stream_.clear();
	}

	std::stringstream &LogHelper::GetStream() 
	{ 
		return string_stream_; 
	}
}

