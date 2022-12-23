/**
 * @file api/Tmdb.hpp
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

#include <atomic>
#include <chrono>
#include <ctime>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

#include "curl.hpp"

namespace TitleFinder {

namespace Api {

class Tmdb {

public:
  [[nodiscard]] static std::shared_ptr<Tmdb> create(const std::string& apiKey);

  /**
   * Destructor
   */
  virtual ~Tmdb();

  [[nodiscard]] std::future<bool> post(std::string_view url,
                                       const nlohmann::json& data);
  [[nodiscard]] std::future<nlohmann::json> get(std::string_view url);
  [[nodiscard]] std::future<bool> del(std::string_view url,
                                      const nlohmann::json& data);

  void setSession(const std::string& id, const std::string& expires);

  void setToken(const std::string& req, const std::string& expires);

  std::string_view getToken() const;

  bool sessionExpired() const;

  bool tokenExpired() const;

  void escapeString(std::string& str) const;

  std::string escapeString(const std::string& str) const;

private:
  /**
   * Empty constructor
   */
  explicit Tmdb(const std::string& apiKey);

  std::string _apiKey;
  std::string _token;
  std::chrono::system_clock::time_point _tokenExpires;
  std::string _sessionId;
  std::chrono::system_clock::time_point _sessionExpires;

  Curl _curl;
};

} // namespace Api

} // namespace TitleFinder
