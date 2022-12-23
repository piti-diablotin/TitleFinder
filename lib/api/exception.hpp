/**
 * @file api/exception.hpp
 *
 * @brief Brief definition of some useful exception
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2022 Jordan Bieder
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

#include <exception>
#include <string>

namespace TitleFinder {

namespace Api {

class Exception : public std::exception {
public:
  const char *what() const noexcept final { return _what.c_str(); }

protected:
  explicit Exception(const std::string &what) : std::exception(), _what(what) {}
  explicit Exception(const char *what) : std::exception(), _what(what) {}

private:
  std::string _what;
};

#define DEFINE_EXCEPTION(NAME)                                                 \
  class NAME : public Exception {                                              \
  public:                                                                      \
    explicit NAME(const std::string &what) : Exception(what) {}                \
    explicit NAME(const char *what) : Exception(what) {}                       \
  };

DEFINE_EXCEPTION(Error401)
DEFINE_EXCEPTION(Error404)
} // namespace Api

} // namespace TitleFinder
