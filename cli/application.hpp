/**
 * @file cli/Application.h
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

#include "parser.hpp"

namespace TitleFinder {

namespace Cli {

class Application {

public:
  /**
   * Empty constructor
   */
  explicit Application(int argc, char* argv[]);

  /**
   * Destructor
   */
  virtual ~Application() = default;

  /**
   * Copy constructor
   * @param application The Application object to copy
   */
  Application(const Application& application) = delete;

  /**
   * Move constructor
   * @param application The Application object to Move
   */
  Application(Application&& application) = delete;

  /**
   * Copy operator
   * @param application The Application object to copy
   * @return A new Application copied from application
   */
  Application& operator=(const Application& application) = delete;

  /**
   * Move operator
   * @param application The Application object to move into the new Application
   * The argument is no more usable after the operation.
   * @return A new Application built from application
   */
  Application& operator=(Application&& application) = delete;

  virtual int run() = 0;

protected:
  Parser _parser;
  int _columns;
};

} // namespace Cli

} // namespace TitleFinder
