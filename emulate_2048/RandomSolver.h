#pragma once

#include <boost\optional.hpp>
#include "Board.h"
#include <random>
#include <chrono>
#include <thread>
#include <string>

namespace Game_2048 {
	class RandomSolver
	{
	public:
		std::string get_name() const { return "RandomSolver"; }
		boost::optional<Action> gen_next_step(const Board&)
		{
			// same pattern as in Cell.cpp
			static thread_local std::mt19937 generator(
				static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())
			);
			std::uniform_int_distribution<int> distribution(0, 3);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			return static_cast<Action>(distribution(generator));
		}
	};
}

