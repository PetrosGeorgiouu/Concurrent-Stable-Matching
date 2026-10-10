#pragma once
#include <vector>

std::vector<std::size_t> GaleShapley(const std::vector<std::vector<std::size_t>> &malePrefs,
                                     std::vector<std::vector<std::size_t>> &femalePrefs);