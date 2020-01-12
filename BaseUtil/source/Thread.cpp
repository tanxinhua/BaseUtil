#include <TaskLoop.h>
#include "UBThread.h"
#ifdef _WIN32
#else
#include <unistd.h>
#endif

namespace BaseUtil
{
	class PrivateThread {
	public:
		static void LoopRun(void *arg);
		static void ThreadFuction(void *arg);
		uv_thread_t thread_id_ = NULL;
		TaskLoop loop_;
		std::function<void()> func_;
	};
}

void BaseUtil::PrivateThread::LoopRun(void *arg)
{
	PrivateThread *d_ = (PrivateThread *)arg;
	d_->loop_.RunLoop();
}

void BaseUtil::PrivateThread::ThreadFuction(void *arg)
{
	PrivateThread *d_ = (PrivateThread *)arg;
    if (d_->func_) {
        d_->func_();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseUtil::Thread::Thread():d_(new PrivateThread)
{

}

BaseUtil::Thread::~Thread()
{
	Quit();
}

void BaseUtil::Thread::Start()
{
	if (d_->loop_.IsRuning() || d_->thread_id_ != NULL)
		return;
	uv_thread_create(&d_->thread_id_, PrivateThread::LoopRun, d_.get());
	while (!d_->loop_.IsRuning()) {
#ifdef _WIN32
		Sleep(1);
#else
        sleep(1);
#endif
    }
	return;
}

void BaseUtil::Thread::Quit()
{	
	if (d_->thread_id_ != NULL) {
		d_->loop_.QuitLoop();
		uv_thread_join(&d_->thread_id_);
		d_->thread_id_ = NULL;
	}	
}

BaseUtil::TaskLoop & BaseUtil::Thread::Loop()
{
	return d_->loop_;
}

uv_thread_t BaseUtil::Thread::StartWithFunction(const std::function<void()>& func)
{
	d_->func_ = func;
	uv_thread_create(&d_->thread_id_, PrivateThread::ThreadFuction, d_.get());
	return d_->thread_id_;
}

void BaseUtil::Thread::RunLoop()
{
	d_->loop_.RunLoop();
}

bool BaseUtil::Thread::PostTask(const std::function<void()>& func)
{
	return d_->loop_.PostTask(func);
}



