#pragma once

#include <vector>

namespace BaseUtil {
	namespace Util {
		template<typename T>
		/*!
		 * @class CircularQueue
		 *  no thread safe!
		 */
		class CircularQueue
		{
		public:
			using ItemType = T;
			/*!
			 * @brief CircularQueue
			 * 
			 * \param max_items  one more item is reserved as marker for full q
			 */
			explicit CircularQueue(size_t max_items)
				: max_items_(max_items + 1) 
				, v_(max_items_)
			{
			}

			/*!
			 * @brief PushBack
			 * push back, overrun (oldest) item if no room left
			 * 
			 * \param item
			 */
			void PushBack(T &&item)
			{
				v_[tail_] = std::move(item);
				tail_ = (tail_ + 1) % max_items_;

				if (tail_ == head_) // overrun last item if full
				{
					head_ = (head_ + 1) % max_items_;
					++overrun_counter_;
				}
			}

			/*!
			 * @brief PopFront
			 *	   Pop item from front.
			 *     If there are no elements in the container, the behavior is undefined
			 * \param popped_item
			 */
			void PopFront(T &popped_item)
			{
				popped_item = std::move(v_[head_]);
				head_ = (head_ + 1) % max_items_;
			}
			/*!
			*  @brief Empty
			*  @return bool
			*/
			bool Empty()
			{
				return tail_ == head_;
			}

			/*!
			*  @brief Empty
			*  @return bool
			*/
			bool Full()
			{
				// head is ahead of the tail by 1
				return ((tail_ + 1) % max_items_) == head_;
			}

			/*!
			*  @brief OverrunCounter
			*  @return size_t
			*/
			size_t OverrunCounter() const
			{
				return overrun_counter_;
			}

		private:
			size_t max_items_;
			typename std::vector<T>::size_type head_ = 0;
			typename std::vector<T>::size_type tail_ = 0;
			std::vector<T> v_;
			size_t overrun_counter_ = 0;
		};
	} 
} 
