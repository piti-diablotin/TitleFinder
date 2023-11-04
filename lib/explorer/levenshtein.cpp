/**
 * @file explorer/levenshtein.cpp
 *
 * @brief Implementation of the 2 matrix rows Levensthein algorithm
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2023 Jordan Bieder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "explorer/levenshtein.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace TitleFinder {

unsigned Levenshtein(const std::string& s, const std::string& t) {
  std::vector<unsigned> v(t.size() + 1, 0);
  std::vector<unsigned> w(t.size() + 1, 0);

  std::iota(v.begin(), v.end(), 0);
  for (unsigned i = 0; i < s.size(); ++i) {
    w[0] = i + 1;

    for (unsigned j = 0; j < t.size(); ++j) {
      unsigned deletionCost = v[j + 1] + 1;
      unsigned insertionCost = w[j] + 1;
      unsigned substitutionCost = s[i] == t[j] ? v[j] : v[j] + 1;
      w[j + 1] = std::min({deletionCost, insertionCost, substitutionCost});
    }

    std::swap(v, w);
  }
  return v.back();
}

} // namespace TitleFinder
