#pragma once

#include "Export.h"
#include <functional>
#include <memory>
#include "uv.h"
#include <TaskLoop.h>
namespace BaseUtil
{
    class PrivateThread;
#ifdef _WIN32
	template   class  BASE_UTIL_EXPORT std::shared_ptr<BaseUtil::PrivateThread>;
#endif
	class BASE_UTIL_EXPORT Thread
	{
	public:
		Thread();
		~Thread();
		void Start();
		uv_thread_t StartWithFunction(const std::function<void()> &func);
		void RunLoop();
		bool PostTask(const std::function<void()> &func);
		void Quit();
		TaskLoop& Loop();
	private:
		std::shared_ptr<PrivateThread> d_;
	};
}
