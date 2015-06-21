#include <cassert>
#include <random>
#include <iterator>
#include <algorithm>
#include <chrono>
#include "MatrixView.h"

#include "Board.h"


using namespace std;
using namespace Game_2048;
using namespace Tools;

// TODO: compile time check for forward iterator
template <typename It, typename OutIt, typename Pred>
void find_all(It begin, It end, OutIt out, Pred pred)
{
    while (begin != end)
    {
        if (pred(*begin)) {
            *out = begin;
            ++out;
        }
        ++begin;
    }
}

const Cell* Game_2048::random_empty_cell(const Board& board)
{
    static thread_local mt19937 generator(
        static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())
        );// same pattern as in Cell.cpp
    
    vector <Board::underlying_type::const_iterator> all_empty; // Board is reasonably small

    // TODO: write transform _copy_if in order to get pointers
    find_all(board.get_cells().begin(), board.get_cells().end(), back_inserter(all_empty),
            [](const Cell& cell) { return cell.is_empty();});
    
    if (all_empty.empty())
        return nullptr;

    uniform_int_distribution<int> distribution(0, all_empty.size() - 1);
    return &(*all_empty[distribution(generator)]); // converting iterators into pointera
}

/*
  I would like to print smth like
  2048 1024 _    _
  _    2    _    _
  _    _    _    _
*/

string Board::to_string() const
{
    using namespace std;
    using namespace PrintingProperties;

    unsigned expected_size = cells_.size(); // extra space for extra separators
                                            // I haven't counted presisely
    expected_size *= symbols_per_cell_ +
        std::max(row_separator.size(), col_separator.size());

    string res;
    res.reserve(expected_size);

    auto rows = make_square_MatrixView(cells_.begin(), cells_.end()).get_all_rows();

    auto print_row = 
        [this, &res](auto row) {
        auto last_col = row->end() - 1;
        for (auto cell = row->begin(); cell != last_col; ++cell)
        {
            res += cell->to_string(symbols_per_cell_);
            res += col_separator;
        }
        res += last_col->to_string();
    };

    auto last_row = rows.end() - 1;
    for (auto row = rows.begin(); row != last_row; ++row)
    {
        print_row(row);
        res += row_separator;
    }
    print_row(last_row);

    return res;
}

template <typename Iter>
void squise_combinable(Iter first, Iter last)
{
    Iter original_first = first;
    if (last - first < 2)
        return;
    vector<typename std::iterator_traits<Iter>::value_type> builder(last - first, Cell());
    for (auto builder_it = builder.begin(); first != last; ++builder_it) {
        auto new_elem = combine(*first, *(first + 1));
        if (!new_elem) {
            *builder_it = *first;
            ++first;
            continue;
        }
        *builder_it = new_elem->first;
        ++first;
        if (first == last)
            break;
        ++first;
    }
    copy(builder.begin(), builder.end(), original_first);
}

template <typename LinesType> // Rows or cols
bool process_lines(LinesType lines)
{
    bool res = false;
    for (auto line : lines) {
        vector<Cell> old_2_compare = { line.begin(), line.end() }; // TODO: can I establish, 
                                                                      // that it was a successfull move faster?
        auto division = stable_partition(line.begin(), line.end(), [](const Cell& cell) { return !cell.is_empty(); });
        squise_combinable(line.begin(), division);
        auto difference = mismatch(old_2_compare.begin(), old_2_compare.end(), line.begin(), line.end()).first;
        res |= (difference != old_2_compare.end());
    }
    return res;
}

Board::success_status Board::apply_action(Action action)
{
    auto board_view = make_square_MatrixView (cells_.begin(), cells_.end());
    auto reverse_board_view = make_square_MatrixView(cells_.rbegin(), cells_.rend());
    switch (action)
    {
    case Action::Up:
        return process_lines(board_view.get_all_cols());
    case Action::Left:
        return process_lines(board_view.get_all_rows());
    case Action::Down:
        return process_lines(reverse_board_view.get_all_cols());
    case Action::Right:
        return process_lines(reverse_board_view.get_all_rows());
    }

    return false;
}
