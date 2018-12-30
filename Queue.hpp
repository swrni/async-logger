#pragma once

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>

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

// Check: http://ithare.com/implementing-queues-for-event-driven-programs/
template <typename Item, int max_size, typename Container = std::vector<Item>>
class Queue {
	public:
		Queue() : items_( max_size ) { items_.resize( max_size ); }
		void Add( Item item ) {
			{
				std::unique_lock<std::mutex> lock( item_list_mutex_ );

				while ( !items_[ add_index_ ].empty_ ) {
					item_popped_cw_.wait( lock );
				}
				items_[ add_index_++ ] = ItemWrapper{ item, false };
				if ( add_index_ == max_size ) {
					add_index_ = 0;
				}
			}
			item_added_cw_.notify_one();
		}

        Item Pop() {
            Item item_to_return;
			{
				std::unique_lock<std::mutex> lock( item_list_mutex_ );

				while ( items_[ pop_index_ ].empty_ ) {
					item_added_cw_.wait( lock );
				}
				auto& item_ref = items_[ pop_index_++ ];
				if ( pop_index_ == max_size ) {
					pop_index_ = 0;
				}

				item_ref.empty_ = true;
                item_to_return = Item{ item_ref.item_ };
			}
			item_popped_cw_.notify_one();
            return item_to_return;
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