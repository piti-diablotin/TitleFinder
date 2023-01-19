/**
 * @file cli/rename.hpp
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

#include <string>

#include "explorer/engine.hpp"

namespace TitleFinder {

namespace Cli {

class Rename final : public Application {

public:
  /**
   * Empty constructor
   */
  explicit Rename(int argc, char* argv[]);

  /**
   * Destructor
   */
  virtual ~Rename() final = default;

  int run() final;

protected:
  virtual int prepare();
  virtual int readyEngine();
  virtual void print(const Explorer::Engine::Prediction& prediction);

private:
  std::string _filename;
  std::filesystem::path _outputDirectory;
  Media::FileInfo::Container _container;
  Explorer::Engine _engine;
};

} // namespace Cli

} // namespace TitleFinder
