#pragma once
#include "Export.h"
#include <functional>
#include<memory>

typedef struct uv_loop_s uv_loop_t;

namespace BaseUtil
{
	class PrivateTimer;
#ifdef _WIN32
	template   class  BASE_UTIL_EXPORT std::shared_ptr<BaseUtil::PrivateTimer>;
#endif
	class BASE_UTIL_EXPORT Timer
	{
	public:
		Timer(uv_loop_t *loop = nullptr);
		~Timer();
		void SetFunction(std::function<void()> func);
		void Start(std::function<void()> func, int time, int repeat = -1);
		void Start(int time, int repeat = -1);
		void Stop();
	private:
		std::shared_ptr<PrivateTimer> d_;
	};
}

#include <vector>
#include <string>
BASE_UTIL_EXPORT std::string TestStringMemory();
BASE_UTIL_EXPORT std::vector<std::string>TestMemory();
BASE_UTIL_EXPORT void TestMemory1(std::vector<std::string> &vec);
BASE_UTIL_EXPORT void TestMemory2(const std::vector<std::string> &vec);
