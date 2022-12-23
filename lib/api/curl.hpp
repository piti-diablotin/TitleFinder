/**
 * @file api/curl.hpp
 *
 * @brief
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2022 Jordan Bieder
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

#include <future>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>

struct curl_slist;

namespace TitleFinder {

namespace Api {

class Curl {

public:
  /**
   * Empty constructor
   */
  explicit Curl(const std::string &baseUrl);

  Curl(const Curl &curl) = delete;
  Curl &operator=(const Curl &curl) = delete;

  /**
   * Destructor
   */
  virtual ~Curl();

  [[nodiscard]] std::future<bool> post(std::string_view url,
                                       const nlohmann::json &data);
  [[nodiscard]] std::future<nlohmann::json> get(std::string_view url);
  [[nodiscard]] std::future<bool> del(std::string_view url,
                                      const nlohmann::json &data);

  static void cleanUp();

  void escapeString(std::string &str) const;

private:
  std::string _baseUrl;
  void *_curl;
  curl_slist *_header;
  std::mutex _resourceUsed;
  static bool _globalInit;
};

} // namespace Api

} // namespace TitleFinder
