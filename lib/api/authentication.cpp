/**
 * @file api/authentication.cpp
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

#include "api/authentication.hpp"
#include "api/logger.hpp"
#include <memory>

namespace TitleFinder {

namespace Api {

Authentication::RequestToken::RequestToken()
    : Response(200), sucess(), expires_at(), request_token() {}

Authentication::Authentication(std::shared_ptr<Tmdb> tmdb) : _tmdb(tmdb) {}

Response_t Authentication::createRequestToken() {
  auto j = _tmdb->get("/authentication/token/new");

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<RequestToken>();
  if (j.contains("success"))
    rep->sucess = j["success"];
  if (j.contains("expires_at"))
    rep->expires_at = j["expires_at"];
  if (j.contains("request_token")) {
    rep->request_token = j["request_token"];
    _tmdb->setToken(rep->request_token.value(), rep->expires_at.value_or(""));
  }
  return rep;
}

} // namespace Api

} // namespace TitleFinder
