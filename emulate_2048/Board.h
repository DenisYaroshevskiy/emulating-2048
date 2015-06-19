#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <array>
#include "Cell.h"
#include <utility>
#include <boost\optional.hpp>

namespace Game_2048 {

	class Board;

	const Cell* random_empty_cell(const Board& board);

	enum class Action { Up, Down, Left, Right };

	inline Cell* random_empty_cell(Board& board)
	{
		// If you haven't seen this, it's a trick from Effective C++
		return const_cast<Cell*>(random_empty_cell(static_cast<const Board&> (board)));
	}

	/*
	 Used assamption - board is reasonably small,
	 It's preferable to copy it over doing
	 some thread safe heap dances.
	*/
	class Board
	{
	public:
		static constexpr int size = 4;
		static constexpr int initial_cells_count = 2;
		
		// std::array would not wor with MatrixView
		// cause VS in DBG mode is not standard conforming
		using underlying_type = boost::array<Cell, size * size>;
		
		std::string to_string() const;
		const underlying_type& get_cells() const { return cells_; }
		Board() {
			for (int i = 0; i < initial_cells_count; ++i) {
				Cell* empty_cell = random_empty_cell(*this);
				assert(empty_cell);
				*empty_cell = Cell::random();
			}
		}
		bool next_step()
		{
			Cell* empty_cell = random_empty_cell(*this);
			if (!empty_cell) return false;
			*empty_cell = Cell::random();
			return true;
		}

		int max() const {
			auto max_iter = std::max_element(cells_.begin(), cells_.end(),
				[](const Cell& lhs, const Cell& rhs) { return lhs.get() < rhs.get();});
			assert(max_iter != cells_.end());
			return max_iter->get();
		}

		using success_status = bool;
		success_status apply_action(Action action);
	private:
		/* FIXME:
		* Replace array with some array_view, cause
		* array of arrays gives an overhead
		*/
		underlying_type cells_;
		int symbols_per_cell_ = 4;
	};

	inline void operator<< (std::ostream& out, const Board& board)
	{
		out << board.to_string();
	}
}

