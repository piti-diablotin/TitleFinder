/**
 * @file explorer/engine.cpp
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

#include "explorer/engine.hpp"

#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>

#include "api/authentication.hpp"
#include "api/search.hpp"
#include "api/structs.hpp"
#include "api/tmdb.hpp"
#include "api/tv.hpp"
#include "api/tvseasons.hpp"
#include "explorer/discriminator.hpp"
#include "explorer/logger.hpp"
#include "media/fileinfo.hpp"
#include "media/tags.hpp"

#define CAST_REPONSE(rep__, type__, dest__)                                    \
  if (rep__->getCode() != 200) {                                               \
    auto* err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep__.get());   \
    throw std::runtime_error(fmt::format(                                      \
        "{}:{}:{}\n Error occured: Error code {}, message {}", __FILE__,       \
        __FUNCTION__, __LINE__, err->getCode(), err->getMessage()));           \
  }                                                                            \
  auto* dest__ = dynamic_cast<type__*>(rep__.get());                           \
  if (dest__ == nullptr) {                                                     \
    throw std::runtime_error(                                                  \
        fmt::format("Unable to cast {} to {}", #rep__, #type__));              \
  }

namespace {
std::regex searchCleaner(R"([\._-]*)");
}

namespace TitleFinder {

namespace Explorer {

Engine::Engine()
    : _tmdb{nullptr}, _language{}, _moviesGenres{},
      _tvShowsGenres{}, _filter{nullptr} {}

void Engine::setTmdbKey(const std::string& key) {
  _tmdb = Api::Tmdb::create(key);
  Api::Authentication auth(_tmdb);
  auto rep = auth.createRequestToken();
  if (rep->getCode() != 200)
    _tmdb.reset();
  CAST_REPONSE(rep, Api::Authentication::RequestToken, token);
  Api::Genres genres(_tmdb);
  try {
    auto gr = genres.getMovieList(_language);
    CAST_REPONSE(gr, Api::Genres::GenresList, mgenre);
    _moviesGenres = std::move(*mgenre);
  } catch (const std::exception& e) {
    Logger()->warn("Unable to retrieve genres for movies.");
  }
  try {
    auto gr = genres.getTvList(_language);
    CAST_REPONSE(gr, Api::Genres::GenresList, sgenre);
    _tvShowsGenres = std::move(*sgenre);
  } catch (const std::exception& e) {
    Logger()->warn("Unable to retrieve genres for TV shows.");
  }
}

void Engine::setLanguage(const std::string& language) {
  const std::regex checkLang("([a-z]{2})-([A-Z]{2})");
  std::smatch m;
  if (std::regex_match(language, m, checkLang)) {
    Logger()->info("Set langue to {} country {}", m[1].str(), m[2].str());
    _language = language;
  } else {
    Logger()->warn("Language format {} not recognized", language);
  }
}

void Engine::setBlacklist(const std::string& blacklistPath) {
  _filter = std::make_unique<NameFilter>(blacklistPath);
}

std::unique_ptr<Api::Search::SearchMovies>
Engine::searchMovie(const std::string& searchString) {
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Search search(_tmdb);
  auto rep = search.searchMovies(_language, searchString, {}, {}, {}, {}, {});
  CAST_REPONSE(rep, Api::Search::SearchMovies, s);
  (void)rep.release();
  return std::unique_ptr<Api::Search::SearchMovies>(s);
}

std::unique_ptr<Api::Search::SearchTvShows>
Engine::searchTvShow(const std::string& searchString) {
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Search search(_tmdb);
  auto rep = search.searchTvShows(_language, {}, searchString, {}, {});
  CAST_REPONSE(rep, Api::Search::SearchTvShows, s);
  (void)rep.release();
  return std::unique_ptr<Api::Search::SearchTvShows>(s);
}

std::unique_ptr<Api::Tv::Details> Engine::getTvShowDetails(int id) {
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Tv show(_tmdb);
  auto rep = show.getDetails(id, _language);
  CAST_REPONSE(rep, Api::Tv::Details, s);
  (void)rep.release();
  return std::unique_ptr<Api::Tv::Details>(s);
}

std::unique_ptr<Api::TvSeasons::Details> Engine::getSeasonDetails(int id,
                                                                  int season) {
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::TvSeasons tvseasons(_tmdb);
  auto rep = tvseasons.getDetails(id, season, _language);
  CAST_REPONSE(rep, Api::TvSeasons::Details, s);
  (void)rep.release();
  return std::unique_ptr<Api::TvSeasons::Details>(s);
}

const Engine::Prediction Engine::predictFile(std::string file) {
  Media::FileInfo info(file);
  if (!std::filesystem::exists(info.getPath()))
    throw std::runtime_error("File {} does not exist.");
  if (_filter) {
    file = _filter->filter(info.getPath().stem());
    Logger()->debug("Title after filtering is {}", file);
  }

  Explorer::Discriminator discri;
  auto t = discri.getType(file);
  std::string title = discri.getTitle();
  Logger()->debug("Discriminator found title {}", title);
  std::replace(title.begin(), title.end(), '.', ' ');
  std::regex_replace(title, searchCleaner, " ",
                     std::regex_constants::match_any);

  auto makeMovie = [this,
                    &info](const std::string& title) -> Engine::Prediction {
    auto rep = this->searchMovie(title);
    if (rep->total_results == 0)
      throw std::logic_error("No match found");
    Prediction pred(std::move(info));
    Api::MovieInfoCompact* tmp =
        new Api::MovieInfoCompact(std::move(rep->results[0]));
    pred.movie.reset(tmp);
    pred.output = fmt::format("{}.({}).{}", pred.movie->title,
                              pred.movie->release_date.substr(0, 4),
                              pred.input.getPath().extension().string());
    Logger()->debug("Movie title found: {}", pred.movie->title);

    return pred;
  };

  if (t == Type::Movie) {
    Logger()->debug("Searching for movie title {}", title);
    try {
      return makeMovie(discri.getTitle());
    } catch (const std::exception& e) {
      using namespace Media::Tag;
      std::string newTest(info.getTag("title"_tagid).data());
      if (newTest.empty())
        throw e;
      Logger()->debug("Searching for movie now with title {}", title);
      return makeMovie(newTest);
    }
  } else if (t == Type::Show) {
    Logger()->debug("Searching for tvshow title {}", title);
    auto rep = this->searchTvShow(title);
    if (rep->total_results == 0)
      throw std::logic_error("No match found");
    Prediction pred(std::move(info));
    Api::TvShowInfoCompact* tmp =
        new Api::TvShowInfoCompact(std::move(rep->results[0]));
    pred.tvshow.reset(tmp);
    Logger()->debug("TvShow title found: {}", pred.tvshow->name);
    Api::TvSeasons season(_tmdb);
    Logger()->debug("Looking for season {} and episode {}", discri.getSeason(),
                    discri.getEpisode());
    auto seasonInfo =
        season.getDetails(pred.tvshow->id, discri.getSeason(), _language);
    CAST_REPONSE(seasonInfo, Api::TvSeasons::Details, details);

    auto ep = std::find_if(details->episodes.begin(), details->episodes.end(),
                           [discri](const Api::Episode& ep) {
                             return ep.episode_number == discri.getEpisode();
                           });

    pred.output = fmt::format("{}.S{:02d}E{:02d}.{}.{}", details->name,
                              ep->season_number, ep->episode_number, ep->name,
                              pred.input.getPath().extension().string());
    Logger()->debug("Episode title is {}", ep->name);
    return pred;
  } else {
    Logger()->debug("Looking for a title tag (assuming movie)");
    using namespace Media::Tag;
    std::string newTest(info.getTag("title"_tagid).data());
    if (!newTest.empty())
      return makeMovie(newTest);
    throw std::logic_error("Unable to predict file");
  }
  return makeMovie("Matrix");
}

void Engine::listFiles(const std::filesystem::path& directory, bool recursive) {
}

void Engine::apply(void) {

  // struct toto {
  //   artist
  //     composer
  //     date
  //     director
  //     description
  //     genre
  //     location
  //     network
  //     publisher
  //     performer
  //     rating
  //     title
  //     year
  //     synopsis
  // }
}

void Engine::autoRename(void* list, Media::FileInfo::Container output,
                        int njobs,
                        const std::filesystem::path& outputDirectory) {}

} // namespace Explorer

} // namespace TitleFinder
