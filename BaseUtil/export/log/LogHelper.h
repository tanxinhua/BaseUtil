#pragma once

#include <sstream>
#include <memory>
#include "Logger.h"
#ifdef _WIN32
#else
#include<unistd.h>
#endif
#include "../Export.h"

#ifdef _WIN32

#endif

namespace BaseUtil
{	
	class  EndLog : public std::string
	{
	};

	const static EndLog endlog;

	class BASE_UTIL_EXPORT LogHelper
	{
	public:
		LogHelper(int level);
		~LogHelper() = default;

		std::stringstream&GetStream();
		void log();
	private:
		std::stringstream string_stream_;
		int level_ = 1;//spdlog::level::level_enum::debug;
	};
}

template<typename T>
extern std::shared_ptr<BaseUtil::LogHelper> operator<<(std::shared_ptr<BaseUtil::LogHelper> logger, T i)
{
	logger->GetStream() << i << " ";
	return logger;
}

template<>
inline std::shared_ptr<BaseUtil::LogHelper> operator<<<BaseUtil::EndLog> (std::shared_ptr<BaseUtil::LogHelper> logger, BaseUtil::EndLog edn_log)
{
	logger->log();
	return logger;
}

template<>
inline std::shared_ptr<BaseUtil::LogHelper> operator<<<std::string> (std::shared_ptr<BaseUtil::LogHelper> logger, std::string string)
{
	logger->GetStream() << string.c_str() << " ";
	return logger;
}

namespace BaseUtil
{
	BASE_UTIL_EXPORT std::shared_ptr<BaseUtil::LogHelper> GetLogHelper(int level);
}

#define LOG_STR_H(x) #x
#define LOG_HELPER_STR(x) LOG_STR_H(x)

#ifdef _WIN32
#define LOG_FMT "["<<(unsigned int)::GetCurrentThreadId()<<"]""[line " LOG_HELPER_STR(__LINE__) "] [" __FUNCTION__ "] "
#else
#define LOG_FMT "[" << (unsigned int) getpid() << "]""[line " << __LINE__ << "] [" << __FUNCTION__<< "] "
#endif

#define LOG_DEBUG BaseUtil::GetLogHelper(1) << LOG_FMT
#define LOG_INFO BaseUtil::GetLogHelper(2) << LOG_FMT
#define LOG_WARNING BaseUtil::GetLogHelper(3) << LOG_FMT
#define LOG_ERROR BaseUtil::GetLogHelper(4) << LOG_FMT
#define LOG_END BaseUtil::endlog
