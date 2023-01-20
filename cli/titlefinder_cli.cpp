#include <memory>

#include "none.hpp"
#include "rename.hpp"
#include "scan.hpp"
#include "search.hpp"

using namespace TitleFinder;

int main(int argc, char** argv) {
  std::unique_ptr<Cli::Application> app;
  if (argc > 2) {
    if (strcmp("search", argv[1]) == 0) {
      app.reset(new Cli::Search(argc - 1, argv + 1));
    } else if (strcmp("rename", argv[1]) == 0) {
      app.reset(new Cli::Rename(argc - 1, argv + 1));
    } else if (strcmp("scan", argv[1]) == 0) {
      app.reset(new Cli::Scan(argc - 1, argv + 1));
    } else {
      app.reset(new Cli::None(argc, argv));
    }
  } else {
    app.reset(new Cli::None(argc, argv));
  }
  return app->run();
}
