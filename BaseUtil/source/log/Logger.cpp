#include "log/Logger.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace BaseUtil
{
	Logger& GetLogger()
	{
		static Logger logger;
		return logger;
	}

	void Logger::init(const std::string &path, const std::string &app_name, const int default_level)
	{
		std::string file_path;
		file_path += path;
		file_path += "/";
		file_path += app_name;
		file_path += ".log";

		std::ifstream file(file_path.c_str());
		if (!file.good())
		{
			std::ofstream outfile(file_path);
			outfile.close();
		}
		spdlog::init_thread_pool(8192, 1);
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::debug);
		console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%P][%l]%v");
		logger_ = spdlog::create_async_nb<spdlog::sinks::rotating_file_sink_mt>("async_file_logger", file_path, 1024 * 1024 * 8, 8);
		logger_->set_level((spdlog::level::level_enum)default_level);
		logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%P][%l]%v");
		logger_->sinks().push_back(console_sink);
	}

	void Logger::log(int level, std::string msg)
	{
		logger_->log((spdlog::level::level_enum)level, msg);
	}

	void Logger::flush()
	{
		logger_->flush();
	}
}











