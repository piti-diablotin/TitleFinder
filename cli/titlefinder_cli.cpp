//#include "api/authentication.hpp"
//#include "api/genres.hpp"
//#include "api/response.hpp"
//#include "api/search.hpp"
//#include "api/tmdb.hpp"
//#include "api/tv.hpp"
//#include "api/tvseasons.hpp"
//#include "explorer/discriminator.hpp"
//#include "explorer/namefilter.hpp"
//#include "media/fileinfo.hpp"
//#include "media/muxer.hpp"
#include "explorer/engine.hpp"
#include "parser.hpp"

#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#ifdef HAVE_IOCTL
#  include <sys/ioctl.h>
#endif

namespace {
std::string getVersion() {
  return fmt::format("{} version {}", TITLEFINDER_NAME, TITLEFINDER_VERSION);
}

// int getVersionMajor() { return TITLEFINDER_VERSION_MAJOR; }
// int getVersionMinor() { return TITLEFINDER_VERSION_MINOR; }
// int getVersionPatch() { return TITLEFINDER_VERSION_PATCH; }

} // namespace

#define CAST_REPONSE(rep__, type__, dest__)                                    \
  if (rep__->getCode() != 200) {                                               \
    auto* err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep__.get());   \
    std::cerr << err->getCode() << std::endl;                                  \
    std::cerr << err->getMessage() << std::endl;                               \
    return -1;                                                                 \
  }                                                                            \
  auto* dest__ = dynamic_cast<type__*>(rep__.get());                           \
  if (dest__ == nullptr) {                                                     \
    std::cerr << "Unable to cast" #rep__ " to " #type__;                       \
    return -1;                                                                 \
  }

using namespace TitleFinder;

int search(int argc, char* argv[]) {
  Parser parser{argc, argv};
  parser.setBinaryName("titlefinder search");
  parser.setOption("help", 'h', "Print this message");
  parser.setOption("api_key", 'k', "API_KEY", "Api key for TheMovieDB");
  parser.setOption("language", 'l', "en-US",
                   "ISO-639-1 language code (e.g. fr-FR)");
  parser.setOption("type", 't', "movie", "Type of research",
                   {"movie", "tvshow"});
  parser.setOption("season", 's', "", "Season number");
  parser.setOption("seasons", 'S', "Print all seasons");
  try {
    parser.parse();
    if (parser.isSetOption("seasons") && parser.isSetOption("season")) {
      std::cerr << "Options \"seasons\" and \"season\" are exclusive."
                << std::endl;
      return 1;
    }
    if (parser.isSetOption("season")) {
      parser.getOption<int>("season");
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  int columns = 80;
#ifdef HAVE_IOCTL
  winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  columns = w.ws_col;
#endif

  if (parser.isSetOption("help")) {
    std::cout << parser << std::endl;
    return 0;
  }

  Explorer::Engine engine;
  try {
    std::string key = parser.getOption<std::string>("api_key");
    engine.setTmdbKey(key);
  } catch (const std::exception& e) {
    std::cerr << "You need to provide an API key" << std::endl;
    return 1;
  }
  engine.setLanguage(parser.getOption<std::string>("language"));

  std::string type = parser.getOption<std::string>("type");
  if (type == "movie") {
    auto result = engine.searchMovie(argv[argc - 1]);
    if (result->total_results == 0) {
      std::cout << "No match found." << std::endl;
      return 1;
    }
    std::cout << fmt::format("|{: ^{}s}|{: ^4s}|{: ^6s}|", "Title",
                             columns - 14, "Year", "Rating")
              << std::endl;
    for (auto& f : result->results) {
      std::cout << fmt::format("|{: <{}s}|{: ^4s}|{: >5.0f}%|", f.title,
                               columns - 14, f.release_date.substr(0, 4),
                               f.vote_average * 10)
                << std::endl;
    }
  } else {
    auto result = engine.searchTvShow(argv[argc - 1]);
    if (result->total_results == 0) {
      std::cout << "No match found." << std::endl;
      return -1;
    }
    if (!parser.isSetOption("seasons") && !parser.isSetOption("season")) {
      std::cout << fmt::format("|{: ^{}s}|{: ^4s}|{: ^6s}|", "Name",
                               columns - 14, "Year", "Rating")
                << std::endl;
      for (auto& f : result->results) {
        std::cout << fmt::format("|{: <{}s}|{: >4s}|{: >5.0f}%|", f.name,
                                 columns - 14, f.first_air_date.substr(0, 4),
                                 f.vote_average * 10)
                  << std::endl;
      }
    } else {
      auto showId = result->results.front().id;
      auto details = engine.getTvShowDetails(showId);
      auto info = fmt::format("{} ({} seasons)", details->name,
                              details->number_of_seasons);
      std::cout << fmt::format("|{: ^{}s}|{: ^10s}|{: ^6s}|", info,
                               columns - 20, "Date", "Rating")
                << std::endl;
      int only = -1;
      if (parser.isSetOption("season"))
        only = parser.getOption<int>("season");
      for (int si = 1; si <= details->number_of_seasons; ++si) {
        if (only > 0 && si != only)
          continue;
        auto season = engine.getSeasonDetails(showId, si);
        std::cout << fmt::format("|{: ^{}s}|{: ^10s}|{: ^6s}|", season->name,
                                 columns - 20, season->air_date, "")
                  << std::endl;
        for (auto& ep : season->episodes) {
          std::cout << fmt::format(
                           "|{: >2d}x{:0>2d} {: <{}s}|{: ^10s}|{: >5.0f}%|",
                           ep.season_number, ep.episode_number, ep.name,
                           columns - 26, ep.air_date, ep.vote_average * 10)
                    << std::endl;
        }
      }
    }
  }
  return 0;
}

int main(int argc, char** argv) {
  Parser parser{argc, argv};
  parser.setOption("help", 'h', "Print this message");
  parser.setOption("version", 'v', "Print help message");

  if (argc == 1) {
    std::cout << parser << std::endl;
    return 1;
  }
  if (argc > 1) {
    if (strcmp("search", argv[1]) == 0) {
      return search(argc, argv);
    } else {

      try {
        parser.parse();
      } catch (const std::exception& e) {
        std::cerr << "Exception occured: " << e.what() << std::endl;
        return 1;
      }

      if (parser.isSetOption("help")) {
        parser.setBinaryName("titlefinder_cli (search|rename|scan)");
        std::cout << parser << std::endl;
      }
      if (parser.isSetOption("version")) {
        std::cout << getVersion() << std::endl;
      }
    }
  }

  //  if (parser.isSetOption("help")) {
  //    std::cout << parser << std::endl;
  //    return 0;
  //  }
  //
  //  std::shared_ptr<Api::Tmdb> tmdb(nullptr);
  //  if (parser.isSetOption("movie") || parser.isSetOption("show")) {
  //    tmdb = Api::Tmdb::create(parser.getOption<std::string>("api_key"));
  //
  //    Api::Authentication auth(tmdb);
  //    Api::Genres genres(tmdb);
  //
  //    auto rep = auth.createRequestToken();
  //    CAST_REPONSE(rep, Api::Authentication::RequestToken, token);
  //
  //    Api::Search search(tmdb);
  //
  //    if (parser.isSetOption("show")) {
  //      rep = search.searchTvShows({}, {},
  //      parser.getOption<std::string>("show"),
  //                                 {}, {});
  //
  //      CAST_REPONSE(rep, Api::Search::SearchTvShows, s);
  //
  //      if (s->results.size() > 0) {
  //        auto& first = s->results[0];
  //        std::cout << first.name << " (" << first.first_air_date.substr(0,
  //        4)
  //                  << ") " << first.vote_average * 10 << "%" << std::endl;
  //
  //        if (!first.genre_ids.empty() && parser.isSetOption("genre")) {
  //          Api::Genres genres(tmdb);
  //          auto gr = genres.getTvList({});
  //          CAST_REPONSE(gr, Api::Genres::GenresList, tvgenre);
  //          for (auto id : first.genre_ids) {
  //            std::cout << tvgenre->genres[id] << ", ";
  //          }
  //          std::cout << std::endl;
  //        }
  //
  //        if (parser.isSetOption("info")) {
  //          Api::Tv tv(tmdb);
  //          auto rep2 = tv.getDetails(first.id, {});
  //          CAST_REPONSE(rep2, Api::Tv::Details, tvshow);
  //          std::cout << tvshow->number_of_seasons << " seasons" <<
  //          std::endl;
  //
  //          Api::TvSeasons tvseasons(tmdb);
  //          for (int si = 1; si <= tvshow->number_of_seasons; ++si) {
  //            auto details = tvseasons.getDetails(first.id, si, {});
  //            if (details->getCode() == 200) {
  //              auto* season =
  //                  dynamic_cast<Api::TvSeasons::Details*>(details.get());
  //              int SN = season->season_number;
  //
  //              size_t len = 0;
  //              for (auto& ep : season->episodes)
  //                if (ep.name.size() > len)
  //                  len = ep.name.size();
  //              std::ostringstream format;
  //              format << "{} S{:02}E{:02} {: <" << len << "s} {:.0f}%";
  //
  //              for (auto& ep : season->episodes) {
  //                std::cout << fmt::format(format.str(), first.name, SN,
  //                                         ep.episode_number, ep.name,
  //                                         ep.vote_average * 10)
  //                          << std::endl;
  //              }
  //            }
  //          }
  //        }
  //      }
  //    }
  //  }
  //
  //  if (parser.isSetOption("file")) {
  //    auto filename = parser.getOption<std::string>("file");
  //    Media::FileInfo file(filename);
  //
  //    if (parser.isSetOption("blacklist")) {
  //      Explorer::NameFilter
  //      filter(parser.getOption<std::string>("blacklist")); filename =
  //      filter.filter(file.getPath().stem());
  //    }
  //
  //    Explorer::Discriminator discri;
  //    auto t = discri.getType(filename);
  //    switch (t) {
  //    case Type::Movie:
  //      std::cout << fmt::format("Discriminator: {} ({})",
  //      discri.getTitle(),
  //                               discri.getYear())
  //                << std::endl;
  //      break;
  //    case Type::Show:
  //      std::cout << fmt::format("Discriminator: {} Season {} Episode {}",
  //                               discri.getTitle(), discri.getSeason(),
  //                               discri.getEpisode())
  //                << std::endl;
  //      break;
  //    default:
  //      std::cout << "Discriminator failed" << std::endl;
  //    }
  //
  //    if (parser.isSetOption("api_key")) {
  //      tmdb = Api::Tmdb::create(parser.getOption<std::string>("api_key"));
  //
  //      Api::Authentication auth(tmdb);
  //      Api::Genres genres(tmdb);
  //
  //      auto rep = auth.createRequestToken();
  //      CAST_REPONSE(rep, Api::Authentication::RequestToken, token);
  //
  //      Api::Search search(tmdb);
  //      if (t == Type::Movie) {
  //        std::string title = discri.getTitle();
  //        std::replace(title.begin(), title.end(), '.', ' ');
  //        rep = search.searchMovies("", title, {}, {}, {}, {}, {});
  //        CAST_REPONSE(rep, Api::Search::SearchMovies, s);
  //        for (auto& f : s->results) {
  //          std::cout << fmt::format("|{}|{}|{:2.0f}%|", f.title,
  //                                   f.release_date.substr(0, 4),
  //                                   f.vote_average * 10)
  //                    << std::endl;
  //        }
  //      } else if (t == Type::Show) {
  //      }
  //    }
  //
  //    if (!file.isOpen())
  //      return 1;
  //
  //    file.dumpInfo();
  //
  //    using namespace Media::Tag;
  //
  //    if (parser.isSetOption("remux")) {
  //      Media::Mp4Muxer muxer(file);
  //      if (parser.isSetOption("year"))
  //        muxer.setTag("year"_tagid, parser.getOption<std::string>("year"));
  //      if (parser.isSetOption("title"))
  //        muxer.setTag("title"_tagid,
  //        parser.getOption<std::string>("title"));
  //      muxer.transmux(parser.getOption<std::string>("remux"));
  //      if (!muxer.isOpen())
  //        return 1;
  //      muxer.dumpInfo();
  //    }
  //  }

  return 0;
}
