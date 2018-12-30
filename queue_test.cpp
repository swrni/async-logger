#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>

#include "Queue.hpp"

int main() {
	std::cout << "At 'queue_test::main()'\n";

	Queue<int, 100> queue;
	for ( int i=0; i<10; ++i ) {
		queue.Add(i*2);
	}
	for ( int i=0; i<10; ++i ) {
		const auto item = queue.Pop();
		std::cout << "Item: " << std::to_string( item ) << '\n';
	}
	
	// system( "pause" );
	return 0;
}