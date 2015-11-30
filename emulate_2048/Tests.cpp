#include "Tests.h"
#include "Board.h"
#include <algorithm>

using namespace Game_2048;
using namespace std;

struct Tests::BoardTests
{
    static bool ok_action(Board before, Board expected_result, Action action)
    {
        if (!before.apply_action(action))
            return false;
        return equal(
            before.cells_.begin(), before.cells_.end(),
            expected_result.cells_.begin(),
            [](Cell lhs, Cell rhs) { return lhs.body_ == rhs.body_;}
        );
    }
    bool static run_one_cell()
    {
        if (!ok_action(
        {
            0, 0, 0, 0,
            2, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
        {
            2, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
            Action::Up))
            return false;

        if (!ok_action(
        {
            0, 0, 0, 2,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
        {
            2, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
            Action::Left))
            return false;

        if (!ok_action(
        {
            0, 0, 0, 0,
            2, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 2,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
            Action::Right))
            return false;

        if (!ok_action(
        {
            0, 0, 0, 0,
            2, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            2, 0, 0, 0
        },
            Action::Down))
            return false;
        return true;
    }
    
    static bool run_2_cells ()
    {
        if (!ok_action(
        {
            2, 0, 0, 0,
            2, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
        {
            4, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        },
        Action::Up))
            return false;

        if (!ok_action(
        {
          0, 2, 0, 2,
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0
        },
        {
          0, 0, 0, 4,
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0
        },
        Action::Right))
            return false;
        if (!ok_action(
        {
          0, 2, 0, 0,
          0, 2, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0
        },
        {
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 4, 0, 0
        },
        Action::Down))
        return false;
      return true;
    }
};

bool Tests::run()
{
    if (!Tests::BoardTests::run_one_cell())
        return false;
    if (!Tests::BoardTests::run_2_cells())
        return false;
    return true;
}



