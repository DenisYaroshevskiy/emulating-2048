#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <boost\variant.hpp>

#include "Solver.h"
#include <initializer_list>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <functional>

namespace Game_2048 {


    /*
     * Alexsanderscu like factory
     * (Modern C++ design)
     */
    template <typename ...SolversTypes>
    class SolversFactory
    {
    public:

        SolversFactory()
        {
            // TODO: asser for keys uniqueness
            // For each possible solver - create a pair of key, 
            // and fuction creator
            // pack them in initializer list
            // that's inserted in a map
            dispatcher_.insert (
            { // main initilizer list
                { // single pair. Pretty neet, ha?)
                    SolversTypes::get_name(),
                    [](Board board)                                       // this a lambda function without capture
                   { return make_solver<SolversTypes>(std::move(board));} // most likely small enough to get stored
                }                                                         // in std::function itself
                ...                                                       
            }
            );
        }

        Solver get_solver(const std::string& key, const Board& board)
        {
            auto found = dispatcher_.find(key);
            if (found == dispatcher_.end())
                throw std::logic_error("Unknow Solver");
            return found->second(board);
        }

        std::vector<std::string> all_solvers_names()
        {
            return
            {
                SolversTypes::get_name()
                ...
            };
        }
    private:
        std::unordered_map<std::string, std::function<Solver(Board)>> dispatcher_;
    };
}

