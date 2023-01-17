/**
 * @file api/genre.cpp
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

#include "api/genres.hpp"

#include "api/logger.hpp"
#include "api/response.hpp"
#include "api/tmdb.hpp"
#include <memory>

namespace {
TitleFinder::Api::Response_t
process(std::shared_ptr<TitleFinder::Api::Tmdb> tmdb,
        const std::string_view url,
        const TitleFinder::Api::optionalString language) {
  std::string options;
  fillEscapeQuery(options, language, tmdb);
  if (options.back() == '&')
    options.pop_back();

  auto j = tmdb->get(
      fmt::format("{}{}{}", url, options.empty() ? "" : "?", options));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<TitleFinder::Api::Genres::GenresList>();
  rep->from_json(j);

  return rep;
}
} // namespace

namespace TitleFinder {

namespace Api {

Genres::GenresList::GenresList() : Response(200), genres() {}

Genres::Genres(std::shared_ptr<Tmdb> tmdb) : _tmdb(tmdb) {}

Response_t Genres::getMovieList(const optionalString language) {
  return process(_tmdb, "/genre/movie/list", language);
}

Response_t Genres::getTvList(const optionalString language) {
  return process(_tmdb, "/genre/tv/list", language);
}
} // namespace Api

} // namespace TitleFinder
