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

#define fillOption(struct__, json__, option__)                                 \
  try {                                                                        \
    if (json__.contains(#option__) && !json__[#option__].is_null())            \
      struct__.option__ = json__[#option__];                                   \
  } catch (...) {                                                              \
    TitleFinder::Api::Logger()->error("Json error for option {}", #option__);  \
  }

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

  if (language.has_value())
    options.append(
        fmt::format("language={}&", _tmdb->escapeString(language.value())));

  options.append(fmt::format("query={}&", _tmdb->escapeString(query)));

  if (page.has_value())
    options.append(fmt::format("page={}&", page.value()));

  if (page.has_value())
    options.append(fmt::format("include_adult={}&",
                               include_adult.value() ? "true" : "false"));

  if (region.has_value())
    options.append(
        fmt::format("region={}&", _tmdb->escapeString(region.value())));

  if (year.has_value())
    options.append(fmt::format("year={}&", year.value()));

  if (primary_release_year.has_value())
    options.append(
        fmt::format("primary_release_year={}&", primary_release_year.value()));

  auto f = _tmdb->get(fmt::format("{}{}", url, options));
  f.wait();
  auto j = f.get();
  Logger()->debug("{}:\n{}", __FUNCTION__, j.dump(2));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<SearchResults<MovieInfo>>();
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

Response_t Search::searchTvShows(optionalString language, optionalInt page,
                                 const std::string &query,
                                 optionalBool include_adult,
                                 optionalInt first_air_date_year) {}

} // namespace Api

} // namespace TitleFinder
