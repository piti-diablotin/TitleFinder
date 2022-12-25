/**
 * @file api/genre.hpp
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

#include <map>
#include <memory>

#include "api/optionals.hpp"
#include "api/response.hpp"
#include "api/structs.hpp"
#include "api/tmdb.hpp"

namespace TitleFinder {

namespace Api {

class Genres {

public:
  class GenresList : public Response {
  public:
    std::map<int, std::string> genres;
    GenresList();
    ~GenresList() = default;
    inline void from_json(const nlohmann::json& j) {
      if (j.contains("genres") && j["genres"].is_array()) {
        auto& g = j["genres"];
        for (size_t i = 0; i < g.size(); ++i) {
          auto& gg = g[i];
          genres.insert(
              std::make_pair(gg.value("id", -1), gg.value("name", "Unknown")));
        }
      }
    }
  };

  /**
   * Empty constructor
   */
  explicit Genres(std::shared_ptr<Tmdb> tmdb);

  /**
   * Destructor
   */
  virtual ~Genres() = default;

  Response_t getMovieList(optionalString language);

  Response_t getTvList(optionalString language);

private:
  std::shared_ptr<Tmdb> _tmdb;
};

} // namespace Api

} // namespace TitleFinder
