/**
 * @file api/tv.cpp
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

#include "api/tv.hpp"

#include "api/logger.hpp"
#include "api/response.hpp"
#include "api/tmdb.hpp"

namespace TitleFinder {

namespace Api {

Tv::Tv(std::shared_ptr<Tmdb> tmdb) : _tmdb(tmdb) {}

Response_t Tv::getDetails(const int tv_id, const optionalString language) {
  std::string url = fmt::format("/tv/{}", tv_id);

  std::string options;

  fillEscapeQuery(options, language, _tmdb);
  if (options.back() == '&')
    options.pop_back();

  auto j = _tmdb->get(fmt::format("{}{}", url, options));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<Details>();
  rep->from_json(j);

  return rep;
}

} // namespace Api

} // namespace TitleFinder
