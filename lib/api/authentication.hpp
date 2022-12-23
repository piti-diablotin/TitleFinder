/**
 * @file api/authentication.hpp
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

#include "api/optionals.hpp"
#include "api/response.hpp"
#include "api/tmdb.hpp"

namespace TitleFinder {

namespace Api {

class Authentication {

public:
  class RequestToken : public Response {
  public:
    optionalBool sucess;
    optionalString expires_at;
    optionalString request_token;
    RequestToken();
    ~RequestToken() = default;
  };

  /**
   * Empty constructor
   */
  explicit Authentication(std::shared_ptr<Tmdb> tmdb);

  /**
   * Destructor
   */
  virtual ~Authentication() = default;

  Response_t createRequestToken();

private:
  std::shared_ptr<Tmdb> _tmdb;
};

} // namespace Api

} // namespace TitleFinder
