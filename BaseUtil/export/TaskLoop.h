#pragma once
#include "Export.h"
#include <functional>
#include <memory>
#include <uv.h>
namespace BaseUtil
{
	using TaskType = enum TaskType {
		AllLevel = -1,
		LowLevel = 0,
		HighLevel,		
	};

	using Task = std::function<void()>;

	using TaskPacket = struct TaskPacket {
		int type_id_;
		Task task_;
	};
    
	class PrivateTaskLoop;
#ifdef _WIN32
	template   class  BASE_UTIL_EXPORT std::shared_ptr<BaseUtil::PrivateTaskLoop>;
#endif
	class BASE_UTIL_EXPORT TaskLoop
	{
	public:
		TaskLoop();
		~TaskLoop();
		void RunLoop();
		bool IsRuning();
		void QuitLoop();
		bool PostTask(const Task& cb, int type_id_ = 0, TaskType type = LowLevel);
		void WakeUp();
		void ClearByTaskType(TaskType type);
		void ClearByTaskId(int type_id = 0);
		uv_loop_t* UVLoop();

	private:
		std::shared_ptr<PrivateTaskLoop> d_;
	};
}
