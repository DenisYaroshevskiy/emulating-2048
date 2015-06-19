#pragma once

#include "Board.h"
#include "Key.h "
#include <boost\optional.hpp>
#include <string>

namespace Game_2048 {
	class HumanSolver
	{
	public:
		std::string get_name() const { return "HumanSolver"; }
		Board get_current_board(Key key) {
			if (key == Key::Other) return board_;
			auto opt_action = gen_next_step(board_, key);
			if (!opt_action)
				ready_ = true;
			else {
				board_.apply_action(*opt_action);
				ready_ = !board_.next_step();
			}
			return board_;
		}
		bool is_ready() const { return ready_; }
		Board get_result() const { return board_; }
	private:
		Board board_;
		bool ready_ = false;
		boost::optional<Action> gen_next_step(const Board&, Key pressed_key)
		{
			switch (pressed_key) {
			case Key::Up:
				return Action::Up;
			case Key::Left:
				return Action::Left;
			case Key::Down:
				return Action::Down;
			case Key::Right:
				return Action::Right;
			default:
				return {};
			}
		}
	};
}

