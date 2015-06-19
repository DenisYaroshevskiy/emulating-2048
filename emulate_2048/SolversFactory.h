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

namespace Game_2048 {
    
    
    /* 
     * Alexsanderscu like factory 
     * (Modern C++ design)
     * Prototype factory
     */
    class SolversFactory
    {
    public:

        SolversFactory(std::initializer_list<Solver> solvers) {
            for (auto& solver : solvers) {                    
                std::string key = solver.get_name();
                assert(dispatcher_.find(key) == dispatcher_.end()); // Registering 2 solvers with 
                                                                    // same name is a bad idea
                dispatcher_.emplace(std::move(key), std::move(solver));
            };

        }

        Solver get_solver(const std::string& key) 
        {
            auto found = dispatcher_.find(key);
            if (found == dispatcher_.end())
                throw std::logic_error("Unknow Solver");
            return found->second;
        }

        std::vector<std::string> all_solvers_names()
        {
            std::vector<std::string> res;
            res.reserve(dispatcher_.size());
            for (const auto& elem : dispatcher_)
                res.push_back(elem.first);
            return res;
        }
    private:
        std::unordered_map<std::string, Solver> dispatcher_;

        void register_solvers() {}


    };
}

