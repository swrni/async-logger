#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <chrono>

#include "Queue.hpp"
#include "RingBuffer.hpp"

/*
Realtime async logger which produces most of the messages in compiletime. Only parts of the messages are edited when executioning the program
*/

struct Message {
    int i;
};

void handle_message( Message message, const bool print_info ) {
    if ( print_info ) {
        std::printf( "consuming: %d\n", message.i );
    }
}

class Context {
    public:
        const int max_int_{ 100 };
        std::atomic<bool> continue_{ true };
        Queue<Message, 1000, std::vector<Message>> messages_;
        int get() {
            // int i = ++i_;
            int i = i_++;
            if ( i >= max_int_ ) {
                continue_ = false;
            }
            return i;
        }
    private:
        std::atomic<int> i_{ 0 };
};

void handle_messages( Context& context, const bool print_info ) {
	while ( context.continue_ ) {
		handle_message( context.messages_.Pop( std::chrono::seconds(10) ), print_info );
	}
    // std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    // std::this_thread::yield();
    // const auto items = context.messages_.Items();
    // std::printf( "items left: %d\n", items );
    // for ( int i=0; i<items; ++i ) {
	// 	handle_message( context.messages_.Pop( std::chrono::seconds(10) ) );
    // }
}

void produce_messages( Context& context, const bool print_info ) {
	while ( context.continue_ ) {
        // const auto sleep_time = std::chrono::seconds(
        //     1 + ( std::rand() % 3 )
        // );
        const auto sleep_time = std::chrono::seconds(0);
		std::this_thread::sleep_for( sleep_time );
        if ( !context.continue_ ) break;
        int i = context.get();
        // std::cout << "creating: " << i << '\n';
        if ( print_info ) {
            std::printf( "  creating: %d\n", i );
        }
        context.messages_.Add( Message{ i }, std::chrono::seconds(5) );
	}
}

template <typename F, typename ...Args>
double Time( F f, Args&&... args ) {
	auto start = std::chrono::steady_clock::now();
	{
        f( std::forward<Args>( args )... );
	}
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration<double, std::milli>( end - start ).count();
}

void RunTest( const bool print_info ) {
	Context context{};
    const int consumer_count{ 1 };
    const int producer_count{ 2 };

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for ( int i=0; i<producer_count; ++i ) {
        producers.emplace_back(
            produce_messages, std::ref( context ), print_info
        );
    }
    for ( int i=0; i<consumer_count; ++i ) {
        consumers.emplace_back(
            handle_messages, std::ref( context ), print_info
        );
    }

    for ( auto& producer : producers ) {
        producer.join();
    }
    for ( auto& consumer : consumers ) {
        consumer.join();
    }
    // handle_messages( context );
    const auto items = context.messages_.Items();
    if ( print_info ) {
        std::printf( "items left: %d\n", items );
    }
    for ( int i=0; i<items; ++i ) {
		handle_message( context.messages_.Pop( std::chrono::seconds(10) ), print_info );
    }
}

int main() {
    const auto time = Time( RunTest, true );
    std::printf( "time: %fms\n", time );
	return 0;
}

