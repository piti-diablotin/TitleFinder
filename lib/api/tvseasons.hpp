/**
 * @file api/tvseasons.hpp
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

class TvSeasons {

public:
  class Details : public Response, public BaseJson {
  public:
    std::string _id;
    std::string air_date;
    std::vector<Episode> episodes;
    std::string name;
    std::string overview;
    int id;
    std::string poster_path;
    int season_number;
    Details();
    ~Details() = default;
    inline void from_json(nlohmann::json& j) {
      fillOption(j, _id);
      fillOption(j, air_date);
      fillOption(j, name);
      fillOption(j, overview);
      fillOption(j, id);
      fillOption(j, poster_path);
      fillOption(j, season_number);
      if (j.contains("episodes") && j["episodes"].is_array()) {
        episodes.resize(j["episodes"].size());
        for (size_t i = 0; i < episodes.size(); ++i) {
          auto& data = j["episodes"][i];
          auto& ep = episodes[i];
          ep.from_json(data);
        }
      }
      _json = std::move(j);
    }
  };

  /**
   * Empty constructor
   */
  explicit TvSeasons(std::shared_ptr<Tmdb> tmdb);

  /**
   * Destructor
   */
  virtual ~TvSeasons() = default;

  Response_t getDetails(int tv_id, int season_number, optionalString language);

private:
  std::shared_ptr<Tmdb> _tmdb;
};

} // namespace Api

} // namespace TitleFinder
