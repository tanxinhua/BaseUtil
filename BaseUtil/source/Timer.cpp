#include <mutex>
#include <atomic>

#include "uv.h"

#include "UBThread.h"

#include "Timer.h"


namespace BaseUtil
{
	using TimerFlag = enum TimerFlag
	{
		None,
		TimerStarted,
		TimerStoped,
	};

	static std::once_flag init_flag;
	static uv_loop_t g_loop;
	static Thread g_thread;
	class PrivateTimer {
	public:
		PrivateTimer();
		~PrivateTimer();
		static void Callback(uv_timer_t *handle);
		static void CloseCallback(uv_handle_t *handle);
        std::atomic<TimerFlag> flag_ {TimerStoped};
		uv_timer_t *uv_timer_ = nullptr;
		uv_loop_t *loop_ = nullptr;
		std::function<void()> func_;
		std::mutex lock_;
	};
}
//if (repeatCount == repeat) {
//	uv_timer_stop(handle);
//	//用完一定要调用uv_close,不然会内存泄露
//	uv_close((uv_handle_t*)handle, NULL);
//}
void BaseUtil::PrivateTimer::Callback(uv_timer_t *handle) {	
	BaseUtil::PrivateTimer* d_ = (BaseUtil::PrivateTimer*)handle->data;
	std::unique_lock<std::mutex> lock(d_->lock_);
	if (d_->func_) {
		d_->func_();
	}
}

void BaseUtil::PrivateTimer::CloseCallback(uv_handle_t *handle) {
	uv_timer_t* d_ = (uv_timer_t*)handle;
	delete d_;
}

BaseUtil::PrivateTimer::PrivateTimer() {

}

BaseUtil::PrivateTimer::~PrivateTimer() {

}

//////////////////////////////////////////////////////////////////////////////////////

BaseUtil::Timer::Timer(uv_loop_t *loop):d_(new BaseUtil::PrivateTimer())
{
	if (loop) {
		d_->loop_ = loop;
	}
	else {
		std::call_once(init_flag, [this]() {
			g_thread.Start();
		});
		d_->loop_= g_thread.Loop().UVLoop();
	}
}

BaseUtil::Timer::~Timer()
{
	if(TimerStarted == d_->flag_)
		Stop();
}

void BaseUtil::Timer::SetFunction(std::function<void()> func)
{
	std::unique_lock<std::mutex> lock(d_->lock_);
	d_->func_ = func;
}

void BaseUtil::Timer::Start(std::function<void()> func, int time, int repeat)
{
	if (d_->flag_ == TimerStarted) {
		Stop();
	}
	SetFunction(func);
	Start(time, repeat);
}

void BaseUtil::Timer::Start(int time, int repeat)
{
	if (d_->flag_ == TimerStarted) {
		Stop();
	}
	std::unique_lock<std::mutex> lock(d_->lock_);
	if (!d_->uv_timer_) {
		d_->uv_timer_ = new uv_timer_t;
	}
	d_->uv_timer_->data = d_.get();
	uv_timer_init(d_->loop_, d_->uv_timer_);
	uv_timer_start(d_->uv_timer_, BaseUtil::PrivateTimer::Callback, time, repeat);
	d_->flag_ = TimerStarted;
	g_thread.Loop().WakeUp();
}

void BaseUtil::Timer::Stop()
{
	if (d_->flag_ != TimerStarted) {
		return;
	}
	std::unique_lock<std::mutex> lock(d_->lock_);
	uv_timer_stop(d_->uv_timer_);
	uv_close((uv_handle_t*)d_->uv_timer_, BaseUtil::PrivateTimer::CloseCallback);
	d_->flag_ = TimerStoped;
	d_->uv_timer_ = nullptr;
}

std::string TestStringMemory()
{
	std::string str("1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
	return str;
}

std::vector<std::string>TestMemory()
{
	std::vector<std::string> vec;
	vec.push_back("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
	//vec.push_back("222222222222222222222222222222222222222222222222222222222222222222222");
	printf("TestMemory vec %p!\n", &vec);
	return vec;
}
BASE_UTIL_EXPORT void TestMemory1(std::vector<std::string> &vec)
{
	vec.push_back("111111111111111111111111111111111111111111111111111111111111111111111");
	printf("TestMemory1 vec %p!\n", &vec);
}
BASE_UTIL_EXPORT void TestMemory2(const std::vector<std::string> &vec)
{
	printf("TestMemory2 vec %p!\n", &vec);
	vec.size();
}
