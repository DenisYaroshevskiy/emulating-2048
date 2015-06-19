// emulate_2048.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include <conio.h>
#include <algorithm>
#include <array>
#include <iterator>
#include "MatrixView.h"

#include "Board.h"
#include "HumanSolver.h"
#include "RandomSolver.h"
#include "AutomaticSolverAdapter.h"
#include "platform_specific.h"
#include "Key.h"

using namespace Game_2048;
using namespace std;
using namespace Tools;

bool test_MatrixView()
{
	array<int, 4> test;
	int inc = 0;
	for (auto &i : test)
		i = inc++;
	copy(test.begin(), test.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	auto matrix_view = make_square_MatrixView(test.begin(), test.end());
	auto column_view = matrix_view.get_col(0);
	copy(column_view.begin(), column_view.end(), ostream_iterator<int>(cout, " "));

	auto row_view = matrix_view.get_row(1);
	cout << endl;
	copy(row_view.begin(), row_view.end(), ostream_iterator<int>(cout, " "));
	return true;
}

void pretty_print(const Board& board, const string& HeadLine = "Board: ")
{
	system("cls");
	cout << HeadLine << endl << endl;
	cout << board.to_string() << endl << endl;
	cout << "Score: " << board.max() << endl;
}

template <typename Solver>
void run_solver(Solver& solver)
{
	for (Key key = Key::Other; !solver.is_ready() && key != Key::Break; key = get_pressed_key()) {
		auto current_board = std::move(solver.get_current_board(key));
		pretty_print(current_board);
	}
	auto result = solver.get_result();
	pretty_print(result, "Final");
}

int main()
{
	AutomaticSolverAdaptor automatic_solver = adapt_automatic_solver<RandomSolver>(Board());
	HumanSolver human_solver;
	run_solver(human_solver);
	wait_any_key();
	return 0;
}

