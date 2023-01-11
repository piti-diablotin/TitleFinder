#include "api/authentication.hpp"
#include "api/genres.hpp"
#include "api/response.hpp"
#include "api/search.hpp"
#include "api/tmdb.hpp"
#include "api/tv.hpp"
#include "api/tvseasons.hpp"
#include "explorer/discriminator.hpp"
#include "explorer/namefilter.hpp"
#include "media/fileinfo.hpp"
#include "media/mkvmux.hpp"
#include "parser.hpp"

#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

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

int main(int argc, char** argv) {
  Parser parser{argc, argv};
  parser.setOption("help", 'h', "Print help message");
  parser.setOption("api_key", 'k', "API_KEY", "Api key for TheMovieDB");
  parser.setOption("movie", 'm', "Matrix", "Movie title (quoted)");
  parser.setOption("show", 's', "Breaking Bad", "TV Show (quoted)");
  parser.setOption("info", 'i', "Print additional information");
  parser.setOption("genre", 'g', "Print additional information");
  parser.setOption("file", 'f', "", "File to read");
  parser.setOption("remux", 'r', "output.mkv", "Output file");
  parser.setOption("title", 't', "",
                   "Title to set in the mkv file if -r is used");
  parser.setOption("year", 'y', "",
                   "Year to set in the mkv file if -r is used");
  parser.setOption("blacklist", 'b', "", "Black list file");
  parser.parse();

  if (parser.isSetOption("help")) {
    std::cout << parser << std::endl;
    return 0;
  }

  std::shared_ptr<Api::Tmdb> tmdb(nullptr);
  if (parser.isSetOption("movie") || parser.isSetOption("show")) {
    tmdb = Api::Tmdb::create(parser.getOption<std::string>("api_key"));

    Api::Authentication auth(tmdb);
    Api::Genres genres(tmdb);

    auto rep = auth.createRequestToken();
    CAST_REPONSE(rep, Api::Authentication::RequestToken, token);

    Api::Search search(tmdb);
    if (parser.isSetOption("movie")) {
      rep = search.searchMovies({}, parser.getOption<std::string>("movie"), {},
                                {}, {}, {}, {});

      CAST_REPONSE(rep, Api::Search::SearchMovies, s);

      if (parser.isSetOption("genre")) {
        auto gr = genres.getMovieList({});
        CAST_REPONSE(gr, Api::Genres::GenresList, mgenre);

        for (auto& f : s->results) {
          std::string fg;
          for (size_t i = 0; i < f.genre_ids.size(); ++i) {
            fg.append(mgenre->genres[f.genre_ids[i]]);
            if (i < f.genre_ids.size() - 1)
              fg.append(", ");
          }
          std::cout << fmt::format("|{}|{}|{}|{:2.0f}%|", f.title,
                                   f.release_date.substr(0, 4), fg,
                                   f.vote_average * 10)
                    << std::endl;
        }
      } else {
        for (auto& f : s->results) {
          std::cout << fmt::format("|{}|{}|{:2.0f}%|", f.title,
                                   f.release_date.substr(0, 4),
                                   f.vote_average * 10)
                    << std::endl;
        }
      }
    }

    if (parser.isSetOption("show")) {
      rep = search.searchTvShows({}, {}, parser.getOption<std::string>("show"),
                                 {}, {});

      CAST_REPONSE(rep, Api::Search::SearchTvShows, s);

      if (s->results.size() > 0) {
        auto& first = s->results[0];
        std::cout << first.name << " (" << first.first_air_date.substr(0, 4)
                  << ") " << first.vote_average * 10 << "%" << std::endl;

        if (!first.genre_ids.empty() && parser.isSetOption("genre")) {
          Api::Genres genres(tmdb);
          auto gr = genres.getTvList({});
          CAST_REPONSE(gr, Api::Genres::GenresList, tvgenre);
          for (auto id : first.genre_ids) {
            std::cout << tvgenre->genres[id] << ", ";
          }
          std::cout << std::endl;
        }

        if (parser.isSetOption("info")) {
          Api::Tv tv(tmdb);
          auto rep2 = tv.getDetails(first.id, {});
          CAST_REPONSE(rep2, Api::Tv::Details, tvshow);
          std::cout << tvshow->number_of_seasons << " seasons" << std::endl;

          Api::TvSeasons tvseasons(tmdb);
          for (int si = 1; si <= tvshow->number_of_seasons; ++si) {
            auto details = tvseasons.getDetails(first.id, si, {});
            if (details->getCode() == 200) {
              auto* season =
                  dynamic_cast<Api::TvSeasons::Details*>(details.get());
              int SN = season->season_number;

              size_t len = 0;
              for (auto& ep : season->episodes)
                if (ep.name.size() > len)
                  len = ep.name.size();
              std::ostringstream format;
              format << "{} S{:02}E{:02} {: <" << len << "s} {:.0f}%";

              for (auto& ep : season->episodes) {
                std::cout << fmt::format(format.str(), first.name, SN,
                                         ep.episode_number, ep.name,
                                         ep.vote_average * 10)
                          << std::endl;
              }
            }
          }
        }
      }
    }
  }

  if (parser.isSetOption("file")) {
    auto filename = parser.getOption<std::string>("file");
    Media::FileInfo file(filename);

    if (parser.isSetOption("blacklist")) {
      Explorer::NameFilter filter(parser.getOption<std::string>("blacklist"));
      filename = filter.filter(file.getPath().stem());
    }

    Explorer::Discriminator discri;
    auto t = discri.getType(filename);
    switch (t) {
    case Type::Movie:
      std::cout << fmt::format("Discriminator: {} ({})", discri.getTitle(),
                               discri.getYear())
                << std::endl;
      break;
    case Type::Show:
      std::cout << fmt::format("Discriminator: {} Season {} Episode {}",
                               discri.getTitle(), discri.getSeason(),
                               discri.getEpisode())
                << std::endl;
      break;
    default:
      std::cout << "Discriminator failed" << std::endl;
    }

    if (parser.isSetOption("api_key")) {
      tmdb = Api::Tmdb::create(parser.getOption<std::string>("api_key"));

      Api::Authentication auth(tmdb);
      Api::Genres genres(tmdb);

      auto rep = auth.createRequestToken();
      CAST_REPONSE(rep, Api::Authentication::RequestToken, token);

      Api::Search search(tmdb);
      if (t == Type::Movie) {
        std::string title = discri.getTitle();
        std::replace(title.begin(), title.end(), '.', ' ');
        rep = search.searchMovies("", title, {}, {}, {}, {}, {});
        CAST_REPONSE(rep, Api::Search::SearchMovies, s);
        for (auto& f : s->results) {
          std::cout << fmt::format("|{}|{}|{:2.0f}%|", f.title,
                                   f.release_date.substr(0, 4),
                                   f.vote_average * 10)
                    << std::endl;
        }
      } else if (t == Type::Show) {
      }
    }

    if (!file.isOpen())
      return 1;

    file.dumpInfo();

    using namespace Media::Tag;

    if (parser.isSetOption("remux")) {
      Media::MkvMux muxer(file);
      if (parser.isSetOption("year"))
        muxer.setTag("year"_tagid, parser.getOption<std::string>("year"));
      if (parser.isSetOption("title"))
        muxer.setTag("title"_tagid, parser.getOption<std::string>("title"));
      muxer.transmux(parser.getOption<std::string>("remux"));
      if (!muxer.isOpen())
        return 1;
      muxer.dumpInfo();
    }
  }

  return 0;
}
