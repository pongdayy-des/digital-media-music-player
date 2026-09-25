#ifndef BANNER_H
#define BANNER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "ConsoleColors.h"

namespace Banner {

inline void printBox(const std::vector<std::string>& lines) {
    std::size_t width = 0;
    for (const auto& l : lines) width = std::max(width, l.size());
    width += 4;

    std::cout << Console::BOLD_RED;

    std::cout << "+";
    for (std::size_t i = 0; i < width; ++i) std::cout << "-";
    std::cout << "+\n";

    for (const auto& l : lines) {
        std::cout << "|  " << l;
        for (std::size_t i = l.size(); i < width - 2; ++i) std::cout << " ";
        std::cout << "|\n";
    }

    std::cout << "+";
    for (std::size_t i = 0; i < width; ++i) std::cout << "-";
    std::cout << "+" << Console::RESET << "\n";
}

} // namespace Banner

#endif // BANNER_H