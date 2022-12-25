/**
 * @file api/search.cpp
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

#include "api/search.hpp"

#include <cstdlib>
#include <fmt/format.h>

#include "api/logger.hpp"

using nlohmann::json;

namespace TitleFinder {

namespace Api {

Search::Search(std::shared_ptr<Tmdb> tmdb) : _tmdb(tmdb) {}

Response_t Search::searchMovies(const optionalString language,
                                const std::string& query, optionalInt page,
                                const optionalBool include_adult,
                                const optionalString region, optionalInt year,
                                const optionalInt primary_release_year) {

  const std::string_view url{"/search/movie?"};
  std::string options;

  fillEscapeQuery(options, language, _tmdb);
  options.append(fmt::format("query={}&", _tmdb->escapeString(query)));
  fillQuery(options, page);
  fillQuery(options, include_adult);
  fillEscapeQuery(options, region, _tmdb);
  fillQuery(options, year);
  fillQuery(options, primary_release_year);
  options.pop_back(); // remove trailing &

  auto j = _tmdb->get(fmt::format("{}{}", url, options));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<SearchMovies>();
  rep->from_json(j);

  return rep;
}

Response_t Search::searchTvShows(const optionalString language,
                                 const optionalInt page,
                                 const std::string& query,
                                 const optionalBool include_adult,
                                 const optionalInt first_air_date_year) {
  const std::string_view url{"/search/tv?"};
  std::string options;

  fillEscapeQuery(options, language, _tmdb);
  fillQuery(options, page);

  options.append(fmt::format("query={}&", _tmdb->escapeString(query)));

  fillQuery(options, include_adult);

  fillQuery(options, first_air_date_year);
  options.pop_back(); // remove trailing &

  auto j = _tmdb->get(fmt::format("{}{}", url, options));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<SearchTvShows>();
  rep->from_json(j);

  return rep;
}

} // namespace Api

} // namespace TitleFinder
