#include "parser.hpp"

#include <getopt.h>

#include <cstdlib>
#include <fmt/format.h>
#include <sstream>
#include <stdexcept>
#include <string_view>

//
Parser::Parser(int argc, char** argv)
    : _argc(argc), _argv(argv), _binary(argv[0]), _empty(-10), _options() {}

//
void Parser::parse() {
  auto nbOptions = _options.size();
  struct option* options = nullptr;
  std::string forGetopts = ":"; // Just to be safe
  options = new struct option[++nbOptions];

  size_t num = 0;
  for (auto& opt : _options) {
    if (opt._letter > 0)
      forGetopts += static_cast<char>(opt._letter);
    if (opt._hasArg == 1) {
      options[num].name = opt._name.c_str();
      options[num].has_arg = required_argument;
      options[num].flag = nullptr;
      options[num++].val = opt._letter;
      forGetopts += ":";
    } else {
      options[num].name = opt._name.c_str();
      options[num].has_arg = no_argument;
      options[num].flag = nullptr;
      options[num++].val = opt._letter;
    }
  }
  options[num].name = nullptr;
  options[num].has_arg = 0;
  options[num].flag = nullptr;
  options[num].val = 0;

  int c;
  int n = 0;
  while ((c = getopt_long(_argc, _argv, forGetopts.c_str(), options,
                          nullptr)) != -1) {
    ++n;
    try {
      auto testOpt = _options.begin();
      for (; testOpt != _options.end(); ++testOpt) {
        if (testOpt->_letter == c) { // We've found the option
          testOpt->_value = (testOpt->_hasArg == 1 ? optarg : "true");
          testOpt->_isSet = true;
          break;
        }
      }
      if (testOpt == _options.end()) {
        if (c == ':') {
          throw std::invalid_argument(
              fmt::format("Missing argument for option {}.", n));
          break;
        } else {
          throw std::out_of_range(fmt::format("Unknown option number {}.", n));
          break;
        }
      }
    } catch (std::out_of_range& e) {
      std::cerr << e.what() << std::endl;
    }
  }
  delete[] options;
}

//
void Parser::setOption(std::string name, char letter, std::string description) {
  _options.push_back(
      {false, std::move(name), letter, 0, "false", std::move(description)});
}

//
void Parser::setOption(std::string name, std::string description) {
  _options.push_back(
      {false, std::move(name), --_empty, 0, "false", std::move(description)});
}

//
void Parser::setOption(std::string name, char letter, std::string defaultValue,
                       std::string description) {
  _options.push_back({false, std::move(name), letter, 1,
                      std::move(defaultValue), std::move(description)});
}

//
void Parser::setOption(std::string name, std::string defaultValue,
                       std::string description) {
  _options.push_back({false, std::move(name), --_empty, 1,
                      std::move(defaultValue), std::move(description)});
}

/**
 * Get the option from command line convert to a string.
 * @param option Name of the option.
 * @result If option has an argument then return it as a string.
 */
template <> std::string Parser::getOption(const std::string_view option) {
  for (auto& testOpt : _options) {
    if (testOpt._name == option) {
      if (testOpt._hasArg == 1) {
        return testOpt._value;
      } else {
        throw std::out_of_range(
            fmt::format("Option {} has no argument", option));
      }
    }
  }
  throw std::domain_error(fmt::format("Unknown option {}", option));
}

/**
 * Get the option from command line convert to a bool.
 * @param option Name of the option.
 * @result If option has an argument then return it as a boolean.
 */
template <> bool Parser::getOption(const std::string_view option) {
  for (auto& testOpt : _options) {
    if (testOpt._name == option) {
      if (testOpt._hasArg == 0) {
        return (testOpt._value == "true" ? true : false);
      } else {
        throw std::out_of_range(
            fmt::format("Option {} has no argument", option));
      }
    }
  }
  throw std::domain_error(fmt::format("Unknown option {}", option));
}

//
bool Parser::isSetOption(std::string_view option) {
  for (auto& testOpt : _options) {
    if (testOpt._name == option) {
      return testOpt._isSet;
    }
  }
  throw std::out_of_range(fmt::format("Unknown option {}", option));
}

//
std::ostream& operator<<(std::ostream& out, const Parser& parser) {
  out << "Usage : " << parser._binary << " [-";

  for (auto& opt : parser._options) {
    if (opt._hasArg == 0 && opt._letter > 0) {
      out << static_cast<char>(opt._letter);
    }
  }

  out << "] ";

  for (auto& opt : parser._options) {
    if (opt._hasArg == 0 && opt._letter < 0) {
      out << "[--" << opt._name << "] ";
    }
  }

  for (auto& opt : parser._options) {
    if (opt._hasArg == 1) {
      out << "[-" << static_cast<char>(opt._letter) << " argument] ";
    }
  }
  out << "\n\nOptions are :\n";
  for (auto& opt : parser._options) {
    if (opt._letter > 0)
      out << "  -" << static_cast<char>(opt._letter) << "  --" << opt._name
          << " : \n"
          << "\t" << opt._description << "\n";
    else
      out << "      --" << opt._name << " : \n\t" << opt._description << "\n";
  }
  out << std::endl;
  return out;
}
