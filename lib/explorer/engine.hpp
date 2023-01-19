/**
 * @file explorer/engine.hpp
 *
 * @brief
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2023 Jordan Bieder
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

#include <filesystem>
#include <memory>
#include <queue>
#include <string>

#include "api/genres.hpp"
#include "api/optionals.hpp"
#include "api/search.hpp"
#include "api/structs.hpp"
#include "api/tmdb.hpp"
#include "api/tv.hpp"
#include "api/tvseasons.hpp"
#include "explorer/discriminator.hpp"
#include "explorer/namefilter.hpp"
#include "media/fileinfo.hpp"
#include "media/muxer.hpp"

namespace TitleFinder {

namespace Explorer {

class Engine {

public:
  struct Prediction {
    Media::FileInfo input;
    std::unique_ptr<Api::MovieInfoCompact> movie;
    std::unique_ptr<Api::TvShowInfoCompact> tvshow;
    std::unique_ptr<Api::Episode> episode;
    std::string output;
    Media::FileInfo::Container container;

    explicit Prediction(Media::FileInfo&& in)
        : input(std::move(in)), movie(nullptr), tvshow(nullptr),
          episode(nullptr), output{},
          container(Media::FileInfo::Container::Other) {}
  };

  /**
   * Empty constructor
   */
  Engine();

  /**
   * Destructor
   */
  virtual ~Engine() = default;

  void setTmdbKey(const std::string& key);

  /**
   * language should follow ISO 639-1 like fr-FR
   */
  void setLanguage(const std::string& language);

  void setBlacklist(const std::string& blacklistPath);

  std::unique_ptr<Api::Search::SearchMovies>
  searchMovie(const std::string& search, Api::optionalInt year = {}) const;

  std::unique_ptr<Api::Search::SearchTvShows>
  searchTvShow(const std::string& search, Api::optionalInt year = {}) const;

  std::unique_ptr<Api::Tv::Details> getTvShowDetails(int id) const;

  std::unique_ptr<Api::TvSeasons::Details> getSeasonDetails(int id,
                                                            int season) const;

  const Prediction
  predictFile(std::string file, Media::FileInfo::Container container,
              const std::filesystem::path& outputDirectory) const;

  std::queue<std::filesystem::path>
  listFiles(const std::filesystem::path& directory, bool recursive) const;

  int apply(const Prediction& pred) const;

  void autoRename(std::queue<std::filesystem::path>& queue,
                  Media::FileInfo::Container container, int njobs,
                  const std::filesystem::path& outputDirectory) const;

private:
  std::shared_ptr<Api::Tmdb> _tmdb;
  Api::optionalString _language;
  Api::Genres::GenresList _moviesGenres;
  Api::Genres::GenresList _tvShowsGenres;
  std::unique_ptr<NameFilter> _filter;
  char _spaceReplacement;
};

} // namespace Explorer

} // namespace TitleFinder
