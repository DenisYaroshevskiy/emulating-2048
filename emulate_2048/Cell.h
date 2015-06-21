#pragma once

#include "PrintingProperties.h"
#include "Tests.h"
#include <boost\optional.hpp>
#include <boost\lexical_cast.hpp>
#include <cassert>
#include <utility>

namespace Game_2048 {

    class Cell
    {
    public:
        friend class Tests;
        using underlying_type = unsigned;
        Cell() : body_(0) {}

        //generates 2 or 4 cell
        static Cell random();

        underlying_type get() const { return body_; }

        std::string to_string(unsigned cell_size) const
        {
            //Maybe I should just use a stringstream
            auto res = to_string();
            assert(cell_size >= res.size());

            res.reserve(cell_size);
            int original_size = res.size();
            for (;cell_size - original_size; --cell_size)
                res.push_back(' ');
            return res;
        }

        std::string to_string() const
        {
            if (is_empty())
                return PrintingProperties::empty_cell;
            return boost::lexical_cast<std::string>(body_);
        }

        bool is_empty() const { return !body_; }

        using continue_combining_tag = bool;
        
        using combine_result = boost::optional<std::pair<Cell, continue_combining_tag> >;
        
        friend combine_result combine(const Cell& lhs, const Cell& rhs)
        {
            if (lhs.is_empty() && rhs.is_empty())
                return combine_result();
            if (lhs.is_empty()) return std::make_pair(rhs, true);
            if (rhs.is_empty()) return std::make_pair(lhs, true);
            if (lhs.body_ == rhs.body_)
                return std::make_pair(Cell(lhs.body_ + rhs.body_), false); // Buffer overflow is impossible
                                                                           // due to the game properties
            return combine_result();
        }
        friend bool operator==(const Cell& lhs, const Cell& rhs)
        {
            return lhs.body_ == rhs.body_;
        }
        friend bool operator!=(const Cell& lhs, const Cell& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        Cell(underlying_type body) : body_(body) {}
        underlying_type body_; // 0 - empty cell, otherwise - value
    };
    
}

