#pragma once

#include <string>

namespace Game_2048 {

    namespace PrintingProperties
    {
        /*
        constexpr means that I have to use const char str[],
        which means, that, for example,
        best way to find out length is to do
        end(str) - begin(str) and so on.

        I do not use this strings at compile time,
        so let them construct at run time
        */
        const std::string empty_cell = "_";
        const std::string row_separator = "\n";
        const std::string col_separator = " ";
    }
}