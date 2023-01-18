
#include <iostream>

#include "parser.hpp"
#include "rename.hpp"
#include "search.hpp"

namespace {
std::string getVersion() {
  return fmt::format("{} version {}", TITLEFINDER_NAME, TITLEFINDER_VERSION);
}
} // namespace

using namespace TitleFinder;

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
      Cli::Search search(argc, argv);
      return search.run();
    } else if (strcmp("rename", argv[1]) == 0) {
      Cli::Rename rename(argc, argv);
      return rename.run();
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
