#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>

#include "Queue.hpp"

/*
Realtime async logger which produces most of the messages in compiletime. Only parts of the messages are edited when executioning the program
*/

struct Message {
};

void handle_message( Message message ) {
}

struct Context {
	std::atomic<bool> continue_;
	std::atomic<bool> new_messages_ = false;

	std::condition_variable messages_cw_;

	Queue messages_;
};

void handle_messages( Context context ) {
	while ( context.continue_ ) {
		std::unique_lock<std::mutex> lock( context.message_list_mutex_ );
		context.messages_cv_.wait(
			lock,
			[](){ return context.new_messages_; }
		);
		handle_message( context.messages_.Pop() );
	}
}

void produce_messages( Context context ) {
	while ( context.continue_ ) {
		sleep( std::random( std::time ) % 10 );
		{
			std::scoped_lock lock( context.message_list_mutex_ );
			context.messages_.Add( Message{} );
		}
		context.messages_cv_.notify_one();
	}
}

int main() {
	
	return 0;
}

