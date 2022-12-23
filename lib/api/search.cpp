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

namespace {
template <class R>
void fillSerchResult(TitleFinder::Api::Search::SearchResults<R> &result,
                     json &data) {
  if (data.contains("page"))
    result.page = data["page"];
  if (data.contains("total_pages"))
    result.total_pages = data["total_pages"];
  if (data.contains("total_results"))
    result.total_results = data["total_results"];
}
} // namespace

namespace TitleFinder {

namespace Api {

Search::Search(std::shared_ptr<Tmdb> tmdb) : _tmdb(tmdb) {}

Response_t Search::searchMovies(const optionalString language,
                                const std::string &query, optionalInt page,
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

  auto f = _tmdb->get(fmt::format("{}{}", url, options));
  f.wait();
  auto j = f.get();
  Logger()->debug("{}:\n{}", __FUNCTION__, j.dump(2));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<SearchMoviesResults>();
  fillSerchResult(*rep, j);

  rep->results.resize(rep->total_results.value_or(0));
  for (int i = 0; i < rep->total_results.value_or(0); ++i) {
    auto &movie = j["results"][i];
    MovieInfo &info = rep->results[i];
    fillOption(info, movie, poster_path);
    fillOption(info, movie, adult);
    fillOption(info, movie, overview);
    fillOption(info, movie, release_date);
    fillOption(info, movie, id);
    fillOption(info, movie, original_title);
    fillOption(info, movie, original_language);
    fillOption(info, movie, title);
    fillOption(info, movie, backdrop_path);
    fillOption(info, movie, popularity);
    fillOption(info, movie, vote_count);
    fillOption(info, movie, video);
    fillOption(info, movie, vote_average);
    if (movie.contains("genre_ids")) {
      movie["genre_ids"].get_to(info.genre_ids);
    }
  }

  return rep;
}

Response_t Search::searchTvShows(const optionalString language,
                                 const optionalInt page,
                                 const std::string &query,
                                 const optionalBool include_adult,
                                 const optionalInt first_air_date_year) {
  const std::string_view url{"/search/tv?"};
  std::string options;

  fillEscapeQuery(options, language, _tmdb);
  fillQuery(options, page);

  options.append(fmt::format("query={}&", _tmdb->escapeString(query)));

  fillQuery(options, include_adult);

  fillQuery(options, first_air_date_year);

  auto f = _tmdb->get(fmt::format("{}{}", url, options));
  f.wait();
  auto j = f.get();
  Logger()->debug("{}:\n{}", __FUNCTION__, j.dump(2));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<SearchTvShowsResults>();
  fillSerchResult(*rep, j);

  rep->results.resize(rep->total_results.value_or(0));
  for (int i = 0; i < rep->total_results.value_or(0); ++i) {
    auto &show = j["results"][i];
    TvShowInfo &info = rep->results[i];
    fillOption(info, show, poster_path);
    fillOption(info, show, popularity);
    fillOption(info, show, id);
    fillOption(info, show, backdrop_path);
    fillOption(info, show, vote_average);
    fillOption(info, show, overview);
    fillOption(info, show, first_air_date);
    fillOption(info, show, original_language);
    fillOption(info, show, vote_count);
    fillOption(info, show, name);
    fillOption(info, show, original_name);
    if (show.contains("genre_ids")) {
      show["genre_ids"].get_to(info.genre_ids);
    }
    if (show.contains("origin_country")) {
      show["origin_country"].get_to(info.origin_country);
    }
  }
  return rep;
}

} // namespace Api

} // namespace TitleFinder
