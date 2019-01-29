#pragma once

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <chrono>

/*
*/

/*
size: 3
read_index: 0
write_index: 0
unread_messages: false // read_index != write_index

R    .
	[ ][ ][ ] |
W    ^
*/

template <typename Item>
class SyncItem {
	public:
		Item getCopy() const {
			std::scoped_lock lock( mutex_ );
			return Item{ item_ };
		}
		void set( Item item ) {
			std::scoped_lock lock( mutex_ );
			item_ = item;
		}
	private:
		Item item_;
		std::mutex mutex_;
};

// using Duration = std::chrono::duration<int>;

template <typename TimeType, int TimeCount>
struct Duration {};

using DurationSecond = Duration<std::chrono::seconds, 1>;

#include <exception>
struct TimeOutException : public std::exception {
    const char* what() const throw() {
        return "timeout exceeded.";
    }
};

// Check: http://ithare.com/implementing-queues-for-event-driven-programs/
template <  typename Item,
            int max_size,
            // template<typename,typename>
            // typename DefaultDuration,
            typename Container = std::vector<Item>
            // typename Duration = std::chrono::milliseconds,
            // int Duration = 100
            >
class Queue {
	public:
		Queue() : items_( max_size ) { items_.resize( max_size ); }
        template <typename Duration>
		bool Add( Item item, Duration cw_wait_duration ) {
			{
				std::unique_lock<std::mutex> lock( item_list_mutex_ );

                const auto place_available = [ this ]() {
                    return items_[ add_index_ ].empty_;
                };
                if ( !item_popped_cw_.wait_for( lock, cw_wait_duration, place_available ) ) {
                    return false;
				}

                items_[ add_index_++ ] = ItemWrapper{ item, false };
                if ( add_index_ == max_size ) {
                    add_index_ = 0;
                }
			}
			item_added_cw_.notify_one();
            return true;
		}

        template <typename Duration>
        Item Pop( Duration cw_wait_duration = {} ) {
            Item item_to_return;
			{
				std::unique_lock<std::mutex> lock( item_list_mutex_ );

                const auto item_available = [ this ]() {
                    return !items_[ pop_index_ ].empty_;
                };
                if ( !item_added_cw_.wait_for( lock, cw_wait_duration, item_available ) ) {
                    throw TimeOutException{};
				}

                items_[ pop_index_ ].empty_ = true;
                item_to_return = std::move( items_[ pop_index_ ].item_ );

				if ( ++pop_index_ == max_size ) {
					pop_index_ = 0;
				}
			}
			item_popped_cw_.notify_one();
            return item_to_return;
        }

        int Items() {
            std::scoped_lock lock( item_list_mutex_ );
            if ( add_index_ < pop_index_ ) {
                return add_index_ + max_size - pop_index_;
            }
            return add_index_ - pop_index_;
        }

	private:
        struct ItemWrapper {
			Item item_;
			bool empty_{ true };
        };

		std::condition_variable item_popped_cw_;
		std::condition_variable item_added_cw_;
		std::mutex item_list_mutex_;
		std::vector<ItemWrapper> items_;
        int add_index_{ 0 };
        int pop_index_{ 0 };
};
