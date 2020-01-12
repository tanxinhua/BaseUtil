#pragma once

#include "CircularQueue.h"

#include <condition_variable>
#include <mutex>

namespace BaseUtil {
	namespace Util {
		/*!
		* @class BlockingQueue
		*  multi producer-multi consumer blocking queue.
		*  enqueue(..) - will block until room found to put the new message.
		*  enqueue_nowait(..) - will return immediately with false if no room left in
		*  the queue.
		*  dequeue_for(..) - will block until the queue is not empty or timeout have
		*  passed.
		*/
		template<typename T>
		class BlockingQueue
		{
		public:
			using ItemType = T;
			explicit BlockingQueue(size_t max_items): q_(max_items)
			{
			}

		#ifndef __MINGW32__
			/*!
			* @brief Enqueue
			*  try to enqueue and block if no room left
			*
			* \param item
			*/
			void Enqueue(T &&item)
			{
				{
					std::unique_lock<std::mutex> lock(queue_mutex_);
					pop_cv_.wait(lock, [this] { return !this->q_.full(); });
					q_.push_back(std::move(item));
				}
				push_cv_.notify_one();
			}

			/*!
			* @brief EnqueueNowait
			*  enqueue immediately. overrun oldest message in the queue if no room left.
			*
			* \param item
			*/
			void EnqueueNowait(T &&item)
			{
				{
					std::unique_lock<std::mutex> lock(queue_mutex_);
					q_.push_back(std::move(item));
				}
				push_cv_.notify_one();
			}
			/*!
			* @brief EnqueueNowait
			*  try to dequeue item. if no item found. wait upto timeout and try again
			*  Return true, if succeeded dequeue item, false otherwise
			*
			* \param item
			* \return bool
			*/
			bool DequeueFor(T &popped_item, std::chrono::milliseconds wait_duration)
			{
				{
					std::unique_lock<std::mutex> lock(queue_mutex_);
					if (!push_cv_.wait_for(lock, wait_duration, [this] { return !this->q_.empty(); }))
					{
						return false;
					}
					q_.pop_front(popped_item);
				}
				pop_cv_.notify_one();
				return true;
			}

		#else
			// apparently mingw deadlocks if the mutex is released before cv.notify_one(),
			// so release the mutex at the very end each function.

			void Enqueue(T &&item)
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				pop_cv_.wait(lock, [this] { return !this->q_.full(); });
				q_.push_back(std::move(item));
				push_cv_.notify_one();
			}

			void EnqueueNowait(T &&item)
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				q_.push_back(std::move(item));
				push_cv_.notify_one();
			}

			bool DequeueFor(T &popped_item, std::chrono::milliseconds wait_duration)
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				if (!push_cv_.wait_for(lock, wait_duration, [this] { return !this->q_.empty(); }))
				{
					return false;
				}
				q_.pop_front(popped_item);
				pop_cv_.notify_one();
				return true;
			}

		#endif
			/*!
			*  @brief OverrunCounter
			*  @return size_t
			*/
			size_t OverrunCounter()
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				return q_.overrun_counter();
			}

		private:
			std::mutex queue_mutex_;
			std::condition_variable push_cv_;
			std::condition_variable pop_cv_;
			BaseUtil::Util::CircularQueue<T> q_;
		};
	} 
} 
