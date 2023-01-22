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
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <string_view>
#include <thread>

#include "api/authentication.hpp"
#include "api/optionals.hpp"
#include "api/search.hpp"
#include "api/structs.hpp"
#include "api/tmdb.hpp"
#include "api/tv.hpp"
#include "api/tvseasons.hpp"
#include "explorer/discriminator.hpp"
#include "explorer/logger.hpp"
#include "media/fileinfo.hpp"
#include "media/muxer.hpp"
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

using json = nlohmann::json;

namespace {
std::regex searchCleaner(R"([\._-]*)");

bool mkdir(const std::filesystem::path& p) {
  TitleFinder::Explorer::Logger()->trace("mkir {}", p.string());
  if (!std::filesystem::exists(p)) {
    if (!mkdir(p.parent_path()))
      return false;
    return std::filesystem::create_directory(p);
  }
  return true;
};
constexpr std::string_view kTvDir = "tv";
constexpr std::string_view kTvSeasonsDir = "tvseasons";
constexpr std::string_view kGenresDir = "genres";
constexpr std::string_view kGenresTv = "tvlist.json";
constexpr std::string_view kGenresMovie = "movielist.json";

inline bool validCacheFile(const std::filesystem::path& p) {
  if (!std::filesystem::exists(p)) {
    return false;
  }
  auto now = std::chrono::system_clock::now().time_since_epoch();
  auto write = std::filesystem::last_write_time(p).time_since_epoch();
  return std::chrono::duration_cast<std::chrono::hours>(now - write) >
         std::chrono::hours(24 * 7);
}
} // namespace

namespace TitleFinder {

namespace Explorer {

Engine::Engine()
    : _tmdb{Api::Tmdb::create("")}, _language{}, _moviesGenres{},
      _tvShowsGenres{}, _filter{nullptr}, _cacheDirectory(),
      _spaceReplacement('.') {
  char* test = nullptr;
  test = ::getenv("LC_MESSAGES");
  if (test == nullptr) {
    test = ::getenv("LANGUAGE");
    if (test == nullptr) {
      test = ::getenv("LANG");
    }
  }
  if (test) {
    const std::regex re("[a-z]{2}_[A-Z]{2}");
    std::cmatch m;
    if (std::regex_search(test, m, re)) {
      std::string match(m[0].str());
      match.replace(2, 1, "-");
      this->setLanguage(match);
    }
  }
#ifdef __linux__
  test = getenv("HOME");
  if (test) {
    _cacheDirectory = std::filesystem::path(test) / ".cache/" TITLEFINDER_NAME;
    if (!mkdir(_cacheDirectory)) {
      Logger()->debug("creation of {} failed", _cacheDirectory.string());
      _cacheDirectory.clear();
    } else {
      Logger()->debug("Cache directory: {}", _cacheDirectory.string());
    }
  }
#endif
}

void Engine::setTmdbKey(const std::string& key) {
  if (!key.empty()) {
    _tmdb->setApiKey(key);
  }
  Api::Authentication auth(_tmdb);
  auto rep = auth.createRequestToken();
  CAST_REPONSE(rep, Api::Authentication::RequestToken, token);
}

void Engine::loadGenresTv() {
  const std::filesystem::path cache = _cacheDirectory / kGenresDir / kGenresTv;
  if (validCacheFile(cache)) {
    Logger()->debug("Loading tv genres from {}", cache.string());
    // load from cache
    std::ifstream file(cache, std::ios::in);
    if (file.is_open()) {
      try {
        json j = json::parse(file);
        file.close();
        _tvShowsGenres.from_json(j);
        return;
      } catch (const std::exception& e) {
        Logger()->error("Failed to load tv genres cache file with: {} ",
                        e.what());
      }
    }
  }
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Genres genres(_tmdb);
  try {
    auto gr = genres.getTvList(_language);
    CAST_REPONSE(gr, Api::Genres::GenresList, sgenre);
    _tvShowsGenres = std::move(*sgenre);
    try {
      mkdir(cache.parent_path());
      std::ofstream file(cache, std::ios::out);
      if (file.is_open()) {
        file << _tvShowsGenres.json().dump();
        file.close();
      }
    } catch (const std::exception& e) {
      Logger()->warn("Unable to cache TV shows genres");
    }
  } catch (const std::exception& e) {
    Logger()->warn("Unable to retrieve genres for TV shows.");
  }
}

void Engine::loadGenresMovie() {
  const std::filesystem::path cache =
      _cacheDirectory / kGenresDir / kGenresMovie;
  if (validCacheFile(cache)) {
    Logger()->debug("Loading movie genres from {}", cache.string());
    // load from cache
    std::ifstream file(cache, std::ios::in);
    if (file.is_open()) {
      try {
        json j = json::parse(file);
        file.close();
        _moviesGenres.from_json(j);
        return;
      } catch (const std::exception& e) {
        Logger()->error("Failed to load movie genres cache file with: {} ",
                        e.what());
      }
    }
  }
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Genres genres(_tmdb);
  try {
    auto gr = genres.getMovieList(_language);
    CAST_REPONSE(gr, Api::Genres::GenresList, mgenre);
    _moviesGenres = std::move(*mgenre);
    try {
      mkdir(cache.parent_path());
      std::ofstream file(cache, std::ios::out);
      if (file.is_open()) {
        file << _moviesGenres.json().dump();
        file.close();
      }
    } catch (const std::exception& e) {
      Logger()->warn("Unable to cache Movie genres");
    }
  } catch (const std::exception& e) {
    Logger()->warn("Unable to retrieve genres for movies.");
  }
}

void Engine::setLanguage(const std::string& language) {
  const std::regex checkLang("([a-z]{2})-([A-Z]{2})");
  std::smatch m;
  if (std::regex_match(language, m, checkLang)) {
    Logger()->info("Set language to {} country {}", m[1].str(), m[2].str());
    _language = language;
  } else {
    Logger()->warn("Language format {} not recognized", language);
  }
}

void Engine::setBlacklist(const std::string& blacklistPath) {
  _filter = std::make_unique<NameFilter>(blacklistPath);
}

std::unique_ptr<Api::Search::SearchMovies>
Engine::searchMovie(const std::string& searchString,
                    Api::optionalInt year) const {
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Search search(_tmdb);
  auto rep = search.searchMovies(_language, searchString, {}, {}, {}, year, {});
  CAST_REPONSE(rep, Api::Search::SearchMovies, s);
  (void)rep.release();
  return std::unique_ptr<Api::Search::SearchMovies>(s);
}

std::unique_ptr<Api::Search::SearchTvShows>
Engine::searchTvShow(const std::string& searchString,
                     Api::optionalInt year) const {
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Search search(_tmdb);
  auto rep = search.searchTvShows(_language, {}, searchString, {}, year);
  CAST_REPONSE(rep, Api::Search::SearchTvShows, s);
  (void)rep.release();
  return std::unique_ptr<Api::Search::SearchTvShows>(s);
}

std::unique_ptr<Api::Tv::Details> Engine::getTvShowDetails(int id) const {
  const std::filesystem::path cache =
      _cacheDirectory / kTvDir / fmt::format("{}.json", id);
  if (validCacheFile(cache)) {
    Logger()->debug("Loading TV show details from {}", cache.string());
    // load from cache
    std::ifstream file(cache, std::ios::in);
    if (file.is_open()) {
      try {
        json j = json::parse(file);
        file.close();
        auto s = std::make_unique<Api::Tv::Details>();
        s->from_json(j);
        return s;
      } catch (const std::exception& e) {
        Logger()->error("Failed to load TV show cache file with: {} ",
                        e.what());
      }
    }
  }
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::Tv show(_tmdb);
  auto rep = show.getDetails(id, _language);
  CAST_REPONSE(rep, Api::Tv::Details, s);
  (void)rep.release();
  try {
    mkdir(cache.parent_path());
    std::ofstream file(cache, std::ios::out);
    if (file.is_open()) {
      file << s->json().dump();
      file.close();
    }
  } catch (const std::exception& e) {
    Logger()->warn("Unable to cache TV show details");
  }
  return std::unique_ptr<Api::Tv::Details>(s);
}

std::unique_ptr<Api::TvSeasons::Details>
Engine::getSeasonDetails(int id, int season) const {
  const std::filesystem::path cache =
      _cacheDirectory / kTvSeasonsDir / fmt::format("{}_{}.json", id, season);
  if (validCacheFile(cache)) {
    Logger()->debug("Loading TV season details from {}", cache.string());
    // load from cache
    std::ifstream file(cache, std::ios::in);
    if (file.is_open()) {
      try {
        json j = json::parse(file);
        file.close();
        auto s = std::make_unique<Api::TvSeasons::Details>();
        s->from_json(j);
        return s;
      } catch (const std::exception& e) {
        Logger()->error("Failed to load TV season cache file with: {} ",
                        e.what());
      }
    }
  }
  if (!_tmdb)
    throw std::runtime_error("You need to set an API key first");
  Api::TvSeasons tvseasons(_tmdb);
  auto rep = tvseasons.getDetails(id, season, _language);
  CAST_REPONSE(rep, Api::TvSeasons::Details, s);
  (void)rep.release();
  try {
    mkdir(cache.parent_path());
    std::ofstream file(cache, std::ios::out);
    if (file.is_open()) {
      file << s->json().dump();
      file.close();
    }
  } catch (const std::exception& e) {
    Logger()->warn("Unable to cache TV season details");
  }
  return std::unique_ptr<Api::TvSeasons::Details>(s);
}

const Engine::Prediction
Engine::predictFile(std::string file, Media::FileInfo::Container container,
                    const std::filesystem::path& outputDirectory) const {

  if (!std::filesystem::exists(file))
    throw std::runtime_error("File {} does not exist.");

  if (!std::filesystem::is_directory(outputDirectory)) {
    throw std::runtime_error(fmt::format(
        "Output directory {} is not a directory", outputDirectory.string()));
  }

  std::string original_file{file};
  if (_filter) {
    file = _filter->filter(file);
    Logger()->debug("File after filtering is {}", file);
  }

  Explorer::Discriminator discri;
  auto t = discri.getType(file);
  std::string title = discri.getTitle();
  Logger()->debug("Discriminator found title {} and year {}", title,
                  discri.getYear());
  std::replace(title.begin(), title.end(), '.', ' ');
  std::regex_replace(title, searchCleaner, " ",
                     std::regex_constants::match_any);

  auto makeMovie = [this, &original_file, &discri, &outputDirectory, container](
                       const std::string& title,
                       Media::FileInfo* info = nullptr) -> Engine::Prediction {
    Api::optionalInt year;
    if (discri.getYear() != -1) {
      year = discri.getYear();
    }
    auto rep = this->searchMovie(title, year);
    if (rep->total_results == 0)
      throw std::logic_error("No match found");
    Prediction pred(info == nullptr ? Media::FileInfo{original_file}
                                    : std::move(*info));
    Api::MovieInfoCompact* tmp =
        new Api::MovieInfoCompact(std::move(rep->results[0]));
    pred.movie.reset(tmp);
    Logger()->debug("Movie title found: {}", pred.movie->title);

    pred.output = fmt::format("{}.({}){}", pred.movie->title,
                              pred.movie->release_date.substr(0, 4),
                              pred.input.getPath().extension().string());
    std::replace(pred.output.begin(), pred.output.end(), ' ',
                 _spaceReplacement);

    pred.output = outputDirectory / pred.output;
    switch (container) {
    case Media::FileInfo::Container::Mkv:
      pred.container = Media::FileInfo::Container::Mkv;
      pred.output = (outputDirectory / pred.output).replace_extension(".mkv");
      break;
    case Media::FileInfo::Container::Mp4:
      pred.container = Media::FileInfo::Container::Mp4;
      pred.output = (outputDirectory / pred.output).replace_extension(".mp4");
      break;
    default:
      pred.output = (outputDirectory / pred.output).string();
      break;
    }
    return pred;
  };

  if (t == Type::Movie) {
    Logger()->debug("Searching for movie title {}", title);
    try {
      return makeMovie(title);
    } catch (const std::exception& e) {
      using namespace Media::Tag;
      try {
        Media::FileInfo info{original_file};
        std::string newTest(info.getTag("title"_tagid).data());
        if (newTest.empty())
          throw e;
        Logger()->debug("Searching for movie now with title {}", newTest);
        return makeMovie(newTest, &info);
      } catch (const std::exception& e) {
        Logger()->error("No tag title");
        throw e;
      }
    }
  } else if (t == Type::Show) {
    Logger()->debug("Searching for tvshow title {}", title);
    auto rep = this->searchTvShow(title);
    if (rep->total_results == 0)
      throw std::logic_error("No match found");
    Prediction pred(Media::FileInfo{original_file});
    Api::TvShowInfoCompact* tmp =
        new Api::TvShowInfoCompact(std::move(rep->results[0]));
    pred.tvshow.reset(tmp);
    Logger()->debug("TvShow title found: {}", pred.tvshow->name);
    Api::TvSeasons season(_tmdb);
    Logger()->debug("Looking for season {} and episode {}", discri.getSeason(),
                    discri.getEpisode());
    auto details = this->getSeasonDetails(pred.tvshow->id, discri.getSeason());

    auto ep = std::find_if(details->episodes.begin(), details->episodes.end(),
                           [discri](const Api::Episode& ep) {
                             return ep.episode_number == discri.getEpisode();
                           });
    Logger()->debug("Episode title is {}", ep->name);

    pred.output =
        fmt::format("{0}/{0}.S{1:02d}/{0}.S{1:02d}E{2:02d}.{3}{4}",
                    pred.tvshow->name, ep->season_number, ep->episode_number,
                    ep->name, pred.input.getPath().extension().string());
    std::replace(pred.output.begin(), pred.output.end(), ' ',
                 _spaceReplacement);

    switch (container) {
    case Media::FileInfo::Container::Mkv:
      pred.container = Media::FileInfo::Container::Mkv;
      pred.output = (outputDirectory / pred.output).replace_extension(".mkv");
      break;
    case Media::FileInfo::Container::Mp4:
      pred.container = Media::FileInfo::Container::Mp4;
      pred.output = (outputDirectory / pred.output).replace_extension(".mp4");
      break;
    default:
      pred.output = (outputDirectory / pred.output).string();
      break;
    }

    pred.episode = std::make_unique<Api::Episode>(std::move(*ep));
    return pred;
  } else {
    Logger()->debug("Looking for a title tag (assuming movie)");
    using namespace Media::Tag;
    Media::FileInfo info{original_file};
    std::string newTest(info.getTag("title"_tagid).data());
    if (!newTest.empty())
      return makeMovie(newTest, &info);
    throw std::logic_error("Unable to predict file");
  }

  return makeMovie("Matrix");
}

std::queue<std::filesystem::path>
Engine::listFiles(const std::filesystem::path& directory,
                  bool recursive) const {
  std::queue<std::filesystem::path> queue;
  if (!std::filesystem::is_directory(directory)) {
    Logger()->error("Input directory {} is not a directory",
                    directory.string());
    return queue;
  }

  auto acceptFile = [](const std::filesystem::directory_entry& entry) -> bool {
    if (!entry.is_regular_file())
      return false;
    const std::string extension = entry.path().extension().string();
    if (extension != ".mp4" && extension != ".mkv" && extension != ".avi")
      return false;
    return true;
  };

  if (recursive) {
    for (const auto& entry :
         std::filesystem::recursive_directory_iterator{directory}) {
      if (acceptFile(entry))
        queue.push(entry);
    }
  } else {
    for (const auto& entry : std::filesystem::directory_iterator{directory}) {
      if (acceptFile(entry))
        queue.push(entry);
    }
  }
  return queue;
}

int Engine::apply(const Prediction& pred) const {

  using namespace Media::Tag;

  Media::Muxer* muxer = nullptr;
  auto container = pred.container;
  if (pred.input.getPath() == std::filesystem::path(pred.output))
    Logger()->info("Not transmuxing, tags cannot be set.");
  else if (container == Media::FileInfo::Container::Other) {
    container = pred.input.getContainer();
  }

  switch (container) {
  case Media::FileInfo::Container::Mkv:
    Logger()->trace("Transmuxing to mkv");
    muxer = new Media::MkvMuxer(pred.input);
    break;
  case Media::FileInfo::Container::Mp4:
    Logger()->trace("Transmuxing to mp4");
    muxer = new Media::Mp4Muxer(pred.input);
    break;
  default:
    Logger()->trace("Only renaming file.");
    break;
  }

  if (muxer && pred.movie) {
    muxer->setTag("title"_tagid, pred.movie->title);
    muxer->setTag("date"_tagid, pred.movie->release_date);
    muxer->setTag("year"_tagid, pred.movie->release_date.substr(0, 4));
    muxer->setTag("description"_tagid, pred.movie->overview);
    muxer->setTag("synopsis"_tagid, pred.movie->overview);
    if (!pred.movie->genre_ids.empty()) {
      std::string genres;
      for (auto id : pred.movie->genre_ids) {
        try {
          genres += _moviesGenres.genres.at(id) + "/";
        } catch (const std::exception& e) {
          continue;
        }
      }
      genres.pop_back();
      muxer->setTag("genre"_tagid, genres);
    }
  } else if (muxer && pred.tvshow && pred.episode) {
    muxer->setTag("title"_tagid, pred.episode->name);
    muxer->setTag("date"_tagid, pred.episode->air_date);
    muxer->setTag("year"_tagid, pred.episode->air_date.substr(0, 4));
    muxer->setTag("description"_tagid, pred.episode->overview);
    muxer->setTag("synopsis"_tagid, pred.episode->overview);
    muxer->setTag("album"_tagid, pred.tvshow->name);
    if (!pred.tvshow->genre_ids.empty()) {
      std::string genres;
      for (auto id : pred.tvshow->genre_ids) {
        try {
          genres += _tvShowsGenres.genres.at(id) + "/";
        } catch (const std::exception& e) {
          continue;
        }
      }
      genres.pop_back();
      muxer->setTag("genre"_tagid, genres);
    }
  }

  std::filesystem::path destinationDirectory{pred.output};
  if (!mkdir(destinationDirectory.parent_path())) {
    throw std::runtime_error(
        fmt::format("Creation of path {} failed.",
                    destinationDirectory.parent_path().string()));
  }

  if (muxer) {
    if (muxer->transmux(pred.output)) {
      std::filesystem::remove(pred.input.getPath());
    }
  } else {
    try {
      std::filesystem::rename(pred.input.getPath(), pred.output);
    } catch (const std::exception& e) {
      Logger()->error("Error renaming file {} to {}",
                      pred.input.getPath().string(), pred.output);
      throw e;
    }
  }
  return 0;
}

void Engine::autoRename(std::queue<std::filesystem::path>& queue,
                        Media::FileInfo::Container container, int njobs,
                        const std::filesystem::path& outputDirectory) const {
  std::mutex queue_mutex;
  std::vector<std::thread> workers;
  for (int i = 0; i < njobs; ++i) {
    workers.push_back(std::thread([&] {
      while (!queue.empty()) {
        std::unique_lock lLock(queue_mutex);
        auto file(std::move(queue.front()));
        queue.pop();
        lLock.unlock();
        try {
          auto pred =
              this->predictFile(file.string(), container, outputDirectory);
          Logger()->info("{} => {}", pred.input.getPath().string(),
                         pred.output);
          this->apply(pred);
        } catch (const std::exception& e) {
          Logger()->error("File {} failed with: {}", file.string(), e.what());
        }
      }
    }));
  }
  for (auto& t : workers) {
    t.join();
  }
}

} // namespace Explorer

} // namespace TitleFinder
