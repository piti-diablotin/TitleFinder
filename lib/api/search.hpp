/**
 * @file api/search.hpp
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

class Search {

public:
  struct MovieInfo {
    optionalString poster_path;
    optionalBool adult;
    optionalString overview;
    optionalString release_date;
    std::vector<int> genre_ids;
    optionalInt id;
    optionalString original_title;
    optionalString original_language;
    optionalString title;
    optionalString backdrop_path;
    optionalDouble popularity;
    optionalInt vote_count;
    optionalBool video;
    optionalDouble vote_average;
    MovieInfo()
        : poster_path(), adult(), overview(), release_date(), genre_ids(), id(),
          original_title(), original_language(), title(), backdrop_path(),
          popularity(), vote_count(), video(), vote_average() {}
  };

  struct TvShowInfo {
    optionalString poster_path;
    optionalDouble popularity;
    optionalInt id;
    optionalString backdrop_path;
    optionalDouble vote_average;
    optionalString overview;
    optionalString first_air_date;
    std::vector<std::string> origin_country;
    std::vector<int> genre_ids;
    optionalString original_language;
    optionalInt vote_count;
    optionalString name;
    optionalString original_name;
    TvShowInfo() = default;
  };

  template <class R> class SearchResults : public Response {
  public:
    optionalInt page;
    std::vector<R> results;
    optionalInt total_results;
    optionalInt total_pages;
    SearchResults()
        : Response(200), page(), results(), total_results(), total_pages() {}
    ~SearchResults() = default;
  };

  using SearchMoviesResults = SearchResults<MovieInfo>;
  using SearchTvShowsResults = SearchResults<TvShowInfo>;

  /**
   * Empty constructor
   */
  explicit Search(std::shared_ptr<Tmdb> tmdb);

  /**
   * Destructor
   */
  virtual ~Search() = default;

  Response_t searchMovies(optionalString language, const std::string& query,
                          optionalInt page, optionalBool include_adult,
                          optionalString region, optionalInt year,
                          optionalInt primary_release_year);

  Response_t searchTvShows(optionalString language, optionalInt page,
                           const std::string& query, optionalBool include_adult,
                           optionalInt first_air_date_year);

private:
  std::shared_ptr<Tmdb> _tmdb;
};

} // namespace Api

} // namespace TitleFinder
