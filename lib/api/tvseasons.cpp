/**
 * @file api/tvseasons.cpp
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

#include "api/tvseasons.hpp"

#include <fmt/format.h>
#include <memory>

#include "api/logger.hpp"

namespace TitleFinder {

namespace Api {

TvSeasons::DetailsResults::DetailsResults()
    : Response(200), _id(), air_date("0000-00-00"), episodes(0),
      name("No Name"), overview(), id(-1), poster_path(), season_number(0) {}

TvSeasons::TvSeasons(std::shared_ptr<Tmdb> tmdb) : _tmdb(tmdb) {}

Response_t TvSeasons::getDetails(const int tv_id, const int season_number,
                                 const optionalString language) {
  std::string url = fmt::format("/tv/{}/season/{}", tv_id, season_number);

  std::string options;

  fillEscapeQuery(options, language, _tmdb);
  if (options.back() == '&')
    options.pop_back();

  auto f = _tmdb->get(fmt::format("{}{}", url, options));
  f.wait();
  auto j = f.get();
  Logger()->debug("{}:\n{}", __FUNCTION__, j.dump(2));

  CHECK_RESPONSE(j);

  auto rep = std::make_unique<DetailsResults>();
  rep->from_json(j);

  return rep;
}
} // namespace Api

} // namespace TitleFinder
