#pragma once

/**
 * WARNING
 * This class is only here for convinience and should not be used or copied
 * anywhere else. When https://itrack.barco.com/browse/BAT0007-111 is resolved
 * one can remove this class and use the BTB CmdLine class instead
 */

#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

/**
 * Parser for the input command line.
 */
class Parser {
private:
  /**
   * Define a small structure for conveniency.
   * It is redundante with the struct option that will be used later.
   */
  struct Option {
    bool _isSet;                    ///< true if user defines this options.
    std::string _name;              ///< Long name.
    int _letter;                    ///< Short name.
    int _hasArg;                    ///< Do we expect an arguement.
    std::string _value;             ///< Value of the argument.
    std::string _description;       ///< Value of the argument.
    std::vector<std::string> _enum; ///< allowed value
  };

  int _argc;           ///< Number of input arguments.
  char** _argv;        ///< List of the input arguments.
  std::string _binary; ///< Name of the executable.
  char _empty;         ///< Count for arguments with no shortcut.

  std::vector<Option> _options; ///< List of all options that can be parsed.

public:
  static const int ERCAL =
      (1 << 0); ///< Error number returned if wrong input line.
  static const int EROPT =
      (1 << 1); ///< Error number returned if unknown option.
  static const int ERARG =
      (1 << 2); ///< Error number returned if wrong argument or not usable.

  /**
   * Construct instance from input argc and argv.
   * @param argc number of parameters.
   * @param argv list of parameters.
   */
  Parser(int argc, char** argv);

  /**
   * Copy a parser object.
   * @param parser The object to copy.
   */
  Parser(const Parser& parser) = delete;

  /**
   * Copy a parser object.
   * @param parser The object to copy.
   * @return Can not be created.
   */
  Parser& operator=(const Parser& parser) = delete;

  /**
   * Do nothing
   */
  ~Parser() = default;

  /**
   * Process the input command line.
   */
  void parse();

  /**
   * Set binary name
   */
  void setBinaryName(std::string_view name);

  /**
   * Add an option to the list that can be read from command line
   * This option will have no argument.
   * It is a boolean.
   * @param name Long name that can be used on command line
   * @param letter Short name (a letter) that can be used on command line
   * @param description Brief for the help
   */
  void setOption(std::string name, char letter, std::string description);

  /**
   * Add an option to the list that can be read from command line
   * This option will have no argument and no shortcut.
   * It is a boolean.
   * @param name Long name that can be used on command line
   * @param description Brief for the help
   */
  void setOption(std::string name, std::string description);

  /**
   * Add an option to the list that can be read from command line
   * This option will have an argument.
   * @param name Long name that can be used on command line
   * @param letter Short name (a letter) that can be used on command line
   * @param defaultValue Default value as a string no matter the desired type.
   * @param description Brief for the help
   */
  void setOption(std::string name, char letter, std::string defaultValue,
                 std::string description,
                 std::vector<std::string> choices = {});

  /**
   * Add an option to the list that can be read from command line
   * This option will have an argument and no shortcut.
   * @param name Long name that can be used on command line
   * @param defaultValue Default value as a string no matter the desired type.
   * @param description Brief for the help
   */
  void setOption(std::string name, std::string defaultValue,
                 std::string description,
                 std::vector<std::string> choices = {});

  /**
   * Get the option from command line.
   * @param option Name of the option.
   * @result If option has an argument then return it as template T.
   */
  template <class T> T getOption(const std::string_view option) {
    static_assert((std::is_arithmetic<T>::value),
                  "Typename should be arithmetic");
    for (auto& testOpt : _options) {
      if (testOpt._name == option) {
        if (testOpt._hasArg == 1) {
          std::stringstream str_tmp;
          str_tmp << testOpt._value;
          T rval;
          str_tmp >> rval;
          if (str_tmp.fail()) {
            throw std::runtime_error(
                fmt::format("Fail to read value for option --{} -{}", option,
                            testOpt._letter));
          }
          return rval;
        } else {
          throw std::invalid_argument(
              fmt::format("Option {} has no argument", option));
        }
      }
    }
    throw std::domain_error(fmt::format("Unknown option {}", option));
  }

  /**
   * Get the option from command line.
   * @param option Name of the option.
   * @result If option has no argument, return true if on command line or false.
   */
  bool isSetOption(std::string_view option);

  /**
   * Print the usage message to ostream.
   * @param out The out stream.
   * @param parser The object to print.
   */
  friend std::ostream& operator<<(std::ostream& out, const Parser& parser);
};

template <> bool Parser::getOption(std::string_view option);

template <> std::string Parser::getOption(std::string_view option);

