/**
 * @file cli/none.hpp
 *
 * @brief
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

#pragma once

#include "application.hpp"

namespace TitleFinder {

namespace Cli {

class None final : public Application {

public:
  /**
   * Empty constructor
   */
  None(int argc, char* argv[]);

  /**
   * Destructor
   */
  ~None() final = default;

  int run() final;
};

} // namespace Cli

} // namespace TitleFinder
