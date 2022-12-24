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
#include "api/structs.hpp"
#include "api/tmdb.hpp"

namespace TitleFinder {

namespace Api {

class Search {

public:
  template <class R> class SearchResults : public Response {
  public:
    int page;
    std::vector<R> results;
    int total_results;
    int total_pages;
    SearchResults()
        : Response(200), page(0), results(), total_results(0), total_pages(0) {}
    ~SearchResults() = default;
    inline void from_json(const nlohmann::json& data) {
      page = data.value("page", page);
      total_pages = data.value("total_pages", total_pages);
      total_results = data.value("total_results", total_results);
    }
  };

  using SearchMoviesResults = SearchResults<MovieInfoCompact>;
  using SearchTvShowsResults = SearchResults<TvShowInfoCompact>;

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
