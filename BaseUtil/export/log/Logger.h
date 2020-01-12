#pragma once
#include "../Export.h"

#include <memory>
#include <sstream>

namespace spdlog
{
	class logger;
}

#ifdef _WIN32
template   class  BASE_UTIL_EXPORT std::shared_ptr<spdlog::logger>;
#endif

namespace BaseUtil
{
	class BASE_UTIL_EXPORT Logger
	{
	public:
		Logger() = default;
		~Logger() = default;

		void init(const std::string &path, const std::string &app_name, const int default_level);
		void log(int level, std::string msg);
		void flush();
	private:
		std::shared_ptr<spdlog::logger> logger_;
	};
	BASE_UTIL_EXPORT Logger& GetLogger();
}