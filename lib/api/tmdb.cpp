/**
 * @file api/tmdb.cpp
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

#include "tmdb.hpp"
#include <chrono>
#include <fmt/format.h>
#include <future>
#include <memory>
#include <string_view>
#include <system_error>

namespace {
std::string addApiKey(std::string_view url, const std::string& key) {
  std::string str(url);
  auto it = str.find('?');
  if (it == std::string::npos) {
    return fmt::format("{}?api_key={}", url, key);
  } else {
    return str.insert(it + 1, fmt::format("api_key={}&", key));
  }
}
} // namespace

namespace TitleFinder {

namespace Api {

using json = nlohmann::json;

std::shared_ptr<Tmdb> Tmdb::create(const std::string& apiKey) {
  auto ptr = new Tmdb(apiKey);
  return std::shared_ptr<Tmdb>(ptr);
}

Tmdb::Tmdb(const std::string& apiKey)
    : _apiKey(apiKey), _token(), _tokenExpires(), _sessionId(),
      _sessionExpires(std::chrono::system_clock::now()),
      _curl("https://api.themoviedb.org/3") {}

Tmdb::~Tmdb() {}

std::future<bool> Tmdb::post(const std::string_view url, const json& data) {
  return _curl.post(addApiKey(url, _apiKey), data);
}

std::future<json> Tmdb::get(const std::string_view url) {
  return _curl.get(addApiKey(url, _apiKey));
}

std::future<bool> Tmdb::del(const std::string_view url, const json& data) {
  return _curl.del(addApiKey(url, _apiKey), data);
}

void Tmdb::setSession(const std::string& id,
                      [[maybe_unused]] const std::string& expires) {
  _sessionId = id;
  //_sessionExpires = expires;
}

void Tmdb::setToken(const std::string& req,
                    [[maybe_unused]] const std::string& expires) {
  _token = req;
  //_tokenExpires = expires;
}

std::string_view Tmdb::getToken() const { return _token; }

bool Tmdb::sessionExpired() const {
  return false; //_sessionExpires < std::chrono::system_clock::now();
}

bool Tmdb::tokenExpired() const {
  return false; //_tokenExpires < std::chrono::system_clock::now();
}

void Tmdb::escapeString(std::string& str) const { _curl.escapeString(str); }

std::string Tmdb::escapeString(const std::string& str) const {
  return _curl.escapeString(str);
}

} // namespace Api

} // namespace TitleFinder
