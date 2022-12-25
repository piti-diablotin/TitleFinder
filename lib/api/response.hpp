/**
 * @file api/response.hpp
 *
 * @brief
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

#include <memory>
#include <string>
#include <string_view>

namespace TitleFinder {

namespace Api {

class Response {

public:
  /**
   * Empty constructor
   */
  Response(int code) : _code(code) {}

  /**
   * Destructor
   */
  virtual ~Response() = default;

  int getCode() const { return _code; }

private:
  int _code;
};

class ErrorResponse : public Response {

public:
  /**
   * Empty constructor
   */
  ErrorResponse(int code, const std::string& message)
      : Response(code), _message(message) {
    ;
  }

  /**
   * Destructor
   */
  virtual ~ErrorResponse() = default;

  std::string_view getMessage() const { return _message; }

private:
  std::string _message;
};

using Response_t = std::unique_ptr<Response>;

#define CHECK_RESPONSE(json__)                                                 \
  if (json__.contains("status_code") && json__["status_code"] >= 400) {        \
    return std::make_unique<TitleFinder::Api::ErrorResponse>(                  \
        json__["status_code"],                                                 \
        json__.value("status_message", "No status message"));                  \
  } else if (json__.contains("success")) {                                     \
    if (!json__["success"]) {                                                  \
      if (json__.contains("errors"))                                           \
        return std::make_unique<TitleFinder::Api::ErrorResponse>(              \
            -1, json__["errors"][0]);                                          \
      else                                                                     \
        return std::make_unique<TitleFinder::Api::ErrorResponse>(              \
            -1, json__.value("status_message", "No status/error message"));    \
    }                                                                          \
  }

#define fillQuery(string__, option__)                                          \
  if (option__.has_value())                                                    \
    string__.append(fmt::format("{}={}&", #option__, option__.value()));

#define fillEscapeQuery(string__, option__, escaper__)                         \
  if (option__.has_value())                                                    \
    string__.append(fmt::format("{}={}&", #option__,                           \
                                escaper__->escapeString(option__.value())));

} // namespace Api

} // namespace TitleFinder
