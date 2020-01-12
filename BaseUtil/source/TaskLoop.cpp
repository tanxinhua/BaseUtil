#include <deque>
#include <mutex>
#include <uv.h>
//#include "log\i_local_logger_helper.h"
#include <TaskLoop.h>

namespace BaseUtil
{
	using FlowFlag = enum FlowFlag
	{
		FlowNone,
		FlowRuning,
		FlowEnd
	};
}

class BaseUtil::PrivateTaskLoop {
public:
		PrivateTaskLoop();
		~PrivateTaskLoop();
		static void WakeUp(uv_async_t *handle);
		static void RunTask(uv_async_t *handle);
		void WakeUp();
		void RunTask();
		Task init_func_;
		Task uninit_func_;

		std::deque<BaseUtil::TaskPacket> container_low_;
		std::deque<BaseUtil::TaskPacket> container_high_;
		std::mutex lock_low_;
		std::mutex lock_high_;	
		uv_loop_t thread_loop_;
		uv_async_t async_1_; 
		uv_async_t async_2_;
		FlowFlag flow_flag_ = FlowNone;
};

BaseUtil::PrivateTaskLoop::PrivateTaskLoop() {

}

BaseUtil::PrivateTaskLoop::~PrivateTaskLoop() {

}

void BaseUtil::PrivateTaskLoop::WakeUp(uv_async_t *handle)
{
	;
}

void BaseUtil::PrivateTaskLoop::RunTask(uv_async_t *handle)
{
	BaseUtil::PrivateTaskLoop *d_ = (BaseUtil::PrivateTaskLoop*)handle->data;
	d_->RunTask();
}

void BaseUtil::PrivateTaskLoop::RunTask()
{
	{
		std::deque<BaseUtil::TaskPacket> deque_high;
		lock_high_.lock();
		std::swap(deque_high, container_high_);
		lock_high_.unlock();
		while (deque_high.size() > 0) {
			auto t = deque_high.front();
			t.task_();
			deque_high.pop_front();
		}
	}

	{
		std::deque<BaseUtil::TaskPacket> deque_low;
		lock_low_.lock();
		std::swap(deque_low, container_high_);
		lock_low_.unlock();
		while (deque_low.size() > 0) {
			auto t = deque_low.front();
			t.task_();
			deque_low.pop_front();
		}
	}
}

void BaseUtil::PrivateTaskLoop::WakeUp()
{
	uv_async_send(&async_2_);
}

///////////////////////////////////////////////////////////////////////////////////////////

BaseUtil::TaskLoop::TaskLoop():d_(new PrivateTaskLoop)
{
}

BaseUtil::TaskLoop::~TaskLoop()
{
}

void BaseUtil::TaskLoop::RunLoop()
{
	int ret = uv_loop_init(&d_->thread_loop_);
	//uv_timer_t uv_timer_;
	//uv_timer_init(&d_->thread_loop_, &uv_timer_);
	//uv_timer_start(&uv_timer_, [](uv_timer_t* handle) {
	//	printf("=============");
	//	//uv_timer_stop(handle);
	//	//uv_close((uv_handle_t*)handle, NULL);
	//}, 10000, 3);

	if (ret != 0) {
//		LOG_ERROR << "uv_loop_init init error!ret =" << ret << LOG_END;
		return;
	}
	d_->async_1_.data = (void*)this;
	d_->async_2_.data = (void*)this;
	uv_async_init(&d_->thread_loop_, &d_->async_1_, BaseUtil::PrivateTaskLoop::RunTask);
	uv_async_init(&d_->thread_loop_, &d_->async_2_, BaseUtil::PrivateTaskLoop::WakeUp);
	d_->flow_flag_ = FlowRuning;
	//crp->init_func_ ? crp->init_func_() : nullptr;
	uv_run(&d_->thread_loop_, UV_RUN_DEFAULT);
	//crp->uninit_func_ ? crp->uninit_func_() : nullptr;
	uv_close((uv_handle_t*)&d_->async_1_, NULL);
	uv_close((uv_handle_t*)&d_->async_2_, NULL);
	uv_loop_close(&d_->thread_loop_);
	d_->flow_flag_ = FlowEnd;
}

bool BaseUtil::TaskLoop::IsRuning()
{
	return FlowRuning == d_->flow_flag_ ? true : false;
}

void BaseUtil::TaskLoop::QuitLoop()
{
	uv_stop(&d_->thread_loop_);
}

bool BaseUtil::TaskLoop::PostTask(const Task & cb, int type_id_, TaskType type)
{
	if (!IsRuning()) {
		return false;
	}
	TaskPacket task;
	task.task_ = cb;
	task.type_id_ = type_id_;
	switch (type)
	{
	case BaseUtil::AllLevel:
	case BaseUtil::LowLevel:
	{
		std::unique_lock<std::mutex> locker(d_->lock_low_);
		d_->container_low_.push_back(task);
	}
		break;
	case BaseUtil::HighLevel:
	{
		std::unique_lock<std::mutex> locker(d_->lock_high_);
		d_->container_high_.push_back(task);
	}
		break;
	default:
		break;
	}
	d_->WakeUp();
	return true;
}

void BaseUtil::TaskLoop::WakeUp()
{
	d_->WakeUp();
}

void BaseUtil::TaskLoop::ClearByTaskType(TaskType type)
{
	switch (type)
	{
	case BaseUtil::AllLevel:
	{
		std::unique_lock<std::mutex> locker(d_->lock_low_);
		d_->container_low_.clear();
	}
	{
		std::unique_lock<std::mutex> locker(d_->lock_high_);
		d_->container_high_.clear();
	}
	break;
	case BaseUtil::LowLevel:
	{
		std::unique_lock<std::mutex> locker(d_->lock_low_);
		d_->container_low_.clear();
	}
		break;
	case BaseUtil::HighLevel:
	{
		std::unique_lock<std::mutex> locker(d_->lock_high_);
		d_->container_high_.clear();
	}
		break;
	default:
		break;
	}
}

void BaseUtil::TaskLoop::ClearByTaskId(int type_id)
{
	{
		std::unique_lock<std::mutex> locker(d_->lock_low_);
		std::deque<BaseUtil::TaskPacket>::iterator dequeIter = d_->container_low_.begin();
		for (; dequeIter != d_->container_low_.end();)
		{
			if (dequeIter->type_id_ == type_id)
			{
				++dequeIter;
			}
			else
			{
				dequeIter = d_->container_low_.erase(dequeIter);
			}
		}
	}

	{
		std::unique_lock<std::mutex> locker(d_->lock_high_);
		std::deque<BaseUtil::TaskPacket>::iterator dequeIter = d_->container_high_.begin();
		for (; dequeIter != d_->container_high_.end();)
		{
			if (dequeIter->type_id_ == type_id)
			{
				++dequeIter;
			}
			else
			{
				dequeIter = d_->container_high_.erase(dequeIter);
			}
		}
	}
}

uv_loop_t* BaseUtil::TaskLoop::UVLoop()
{
	return &d_->thread_loop_;
}
