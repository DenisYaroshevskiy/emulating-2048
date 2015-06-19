
#include "Cell.h"
#include <random>
#include <chrono>

using namespace std;
using namespace Game_2048;

Cell Cell::random()
{
	/*
	 * Used engine was suggestested by stephan T lavavej
	 * on going native 2013
	 */
	
	 /* 4 seems to be generated much rearely then 2
	 * Maybe, there is a propper way to simulate this
	 * FIXME: find out, how it should be done
	 */
	static thread_local mt19937 generator (
		static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())
		); // Constracting engines is not cheep, 
				     					   // but not enough to serialise all calls
	const uniform_int_distribution<int> distribution(2, 6); // I should read some more

	int next_random_number = distribution(generator);
	if (next_random_number == 6)
		return Cell(4);
	return Cell(2);
}