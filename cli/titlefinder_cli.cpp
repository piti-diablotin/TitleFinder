#include "api/authentication.hpp"
#include "api/genres.hpp"
#include "api/response.hpp"
#include "api/search.hpp"
#include "api/tmdb.hpp"
#include "api/tvseasons.hpp"
#include "parser.hpp"

#include <iostream>

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

int main(int argc, char** argv) {
  Parser parser{argc, argv};
  parser.setOption("help", 'h', "Print help message");
  parser.setOption("api_key", 'k', "API_KEY", "Api key for TheMovieDB");
  parser.setOption("movie", 'm', "Matrix", "Movie title (quoted)");
  parser.setOption("show", 's', "Breaking Bad", "TV Show (quoted)");
  parser.setOption("info", 'i', "Print additional information");
  parser.setOption("genre", 'g', "Print additional information");
  parser.parse();

  if (parser.isSetOption("help")) {
    std::cout << parser << std::endl;
    return 0;
  }

  auto tmdb =
      TitleFinder::Api::Tmdb::create(parser.getOption<std::string>("api_key"));

  TitleFinder::Api::Authentication auth(tmdb);

  auto rep = auth.createRequestToken();
  CAST_REPONSE(rep, TitleFinder::Api::Authentication::RequestToken, token);

  TitleFinder::Api::Search search(tmdb);
  if (parser.isSetOption("movie")) {
    rep = search.searchMovies({}, parser.getOption<std::string>("movie"), {},
                              {}, {}, {}, {});

    CAST_REPONSE(rep, TitleFinder::Api::Search::SearchMovies, s);

    if (s->results.size() > 0) {
      auto& first = s->results[0];
      std::cout << first.title << " (" << first.release_date.substr(0, 4) << ")"
                << std::endl;
      if (!first.genre_ids.empty() && parser.isSetOption("genre")) {
        TitleFinder::Api::Genres genres(tmdb);
        auto gr = genres.getMovieList({});
        CAST_REPONSE(gr, TitleFinder::Api::Genres::GenresList, mgenre);
        for (auto id : first.genre_ids) {
          std::cout << mgenre->genres[id] << ", ";
        }
        std::cout << std::endl;
      }
    }
  }

  if (parser.isSetOption("show")) {
    rep = search.searchTvShows({}, {}, parser.getOption<std::string>("show"),
                               {}, {});

    CAST_REPONSE(rep, TitleFinder::Api::Search::SearchTvShows, s);

    if (s->results.size() > 0) {
      auto first = s->results[0];
      std::cout << first.name << " (" << first.first_air_date.substr(0, 4)
                << ")" << std::endl;

      if (!first.genre_ids.empty() && parser.isSetOption("genre")) {
        TitleFinder::Api::Genres genres(tmdb);
        auto gr = genres.getTvList({});
        CAST_REPONSE(gr, TitleFinder::Api::Genres::GenresList, tvgenre);
        for (auto id : first.genre_ids) {
          std::cout << tvgenre->genres[id] << ", ";
        }
        std::cout << std::endl;
      }

      if (parser.isSetOption("info")) {
        TitleFinder::Api::TvSeasons tvseasons(tmdb);
        int si = 0;
        while (true) {
          auto details = tvseasons.getDetails(first.id, ++si, {});
          if (details->getCode() == 200) {
            auto* season = dynamic_cast<TitleFinder::Api::TvSeasons::Details*>(
                details.get());
            int SN = season->season_number;
            for (auto& ep : season->episodes) {
              std::cout << fmt::format("{} S{:02}E{:02} {}", first.name, SN,
                                       ep.episode_number, ep.name)
                        << std::endl;
            }
          } else {
            break;
          }
        }
      }
    }
  }

  return 0;
}
