/**
 * @file api/curl.cpp
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

#include "api/curl.hpp"

#include <cstring>
#include <curl/curl.h>
#include <fmt/core.h>
#include <future>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include "api/logger.hpp"

namespace {
size_t writefunction(char* ptr, size_t size, size_t nmemb, void* userData) {
  char* str = static_cast<char*>(ptr);
  static_cast<std::string*>(userData)->append(str, str + (size * nmemb));
  return size * nmemb;
}
} // namespace

namespace TitleFinder {

namespace Api {

using json = nlohmann::json;

bool Curl::_globalInit = false;

Curl::Curl(const std::string& baseUrl)
    : _baseUrl(baseUrl), _curl(nullptr), _header(nullptr), _resourceUsed() {
  if (!_globalInit) {
    Logger()->debug("Init curl globaly");
    curl_global_init(CURL_GLOBAL_ALL);
    _globalInit = true;
  }
  _curl = curl_easy_init();
  if (!_curl) {
    throw std::runtime_error("Unable to init curl instance");
  }
  _header = nullptr; // init to NULL is important
  _header = curl_slist_append(_header, "Accept: application/json");
  _header = curl_slist_append(_header, "Content-Type: application/json");
  _header = curl_slist_append(_header, "charset: utf-8");
  curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header);
  curl_easy_setopt(_curl, CURLOPT_USERAGENT, "TitleFinder");
  curl_easy_setopt(_curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
  curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writefunction);
}

Curl::~Curl() {
  curl_easy_cleanup(_curl);
  curl_slist_free_all(_header);
}

void Curl::cleanUp() {
  Logger()->info("Clean curl globaly");
  curl_global_cleanup();
  _globalInit = false;
}

std::future<json> Curl::post(const std::string_view url, const json& data) {
  std::string fullUrl = fmt::format("{}{}", _baseUrl, url);

  Logger()->debug("Sending post to {}", fullUrl);
  if (!_curl)
    throw std::runtime_error("Bad curl instance");

  return std::async(std::launch::async, [=, &data]() -> json {
    std::lock_guard Log(_resourceUsed);
    char errorStr[CURL_ERROR_SIZE]{0};
    std::string result;

    curl_easy_setopt(_curl, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorStr);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, static_cast<void*>(&result));
    curl_easy_setopt(_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.dump().c_str());
    auto res = curl_easy_perform(_curl);
    if (res != CURLE_OK) {
      return json::parse(fmt::format(
          R"({ status_message: "{}", status_code: {})", errorStr, res));
    }
    try {
      return json::parse(result);
    } catch (const std::exception& e) {
      Logger()->error("Enable to parse json, received data is \n{}", result);
      return json::parse(fmt::format(
          R"({{ "status_message": "json parse error: {}", "status_code": {}}})",
          e.what(), -1));
    } catch (...) {
      Logger()->error("Enable to parse json, received data is \n{}", result);
      return json::parse(fmt::format(
          R"({{ "status_message": "not an std::exception", "status_code": {}}})",
          -2));
    }
  });
}

std::future<json> Curl::get(const std::string_view url) {
  std::string fullUrl = fmt::format("{}{}", _baseUrl, url);

  Logger()->debug("Sending get to {}", fullUrl);
  if (!_curl)
    throw std::runtime_error("Bad curl instance");

  return std::async(std::launch::async, [=]() -> json {
    std::lock_guard Log(_resourceUsed);
    char errorStr[CURL_ERROR_SIZE]{0};
    std::string result;

    curl_easy_setopt(_curl, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorStr);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, static_cast<void*>(&result));
    auto res = curl_easy_perform(_curl);
    if (res != CURLE_OK) {
      return json::parse(fmt::format(
          R"({ status_message: "{}", status_code: {})", errorStr, res));
    }
    try {
      return json::parse(result);
    } catch (const std::exception& e) {
      Logger()->error("Enable to parse json, received data is \n{}", result);
      return json::parse(fmt::format(
          R"({{ "status_message": "json parse error: {}", "status_code": {}}})",
          e.what(), -1));
    } catch (...) {
      Logger()->error("Enable to parse json, received data is \n{}", result);
      return json::parse(fmt::format(
          R"({{ "status_message": "not an std::exception", "status_code": {}}})",
          -2));
    }
  });
}

std::future<json> Curl::del(const std::string_view url, const json& data) {
  std::string fullUrl = fmt::format("{}{}", _baseUrl, url);

  Logger()->debug("Sending delete to {}", fullUrl);
  if (!_curl)
    throw std::runtime_error("Bad curl instance");

  return std::async(std::launch::async, [=, &data]() -> json {
    std::lock_guard Log(_resourceUsed);
    char errorStr[CURL_ERROR_SIZE]{0};
    std::string result;

    curl_easy_setopt(_curl, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorStr);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, static_cast<void*>(&result));
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.dump().c_str());
    auto res = curl_easy_perform(_curl);
    if (res != CURLE_OK) {
      return json::parse(fmt::format(
          R"({ status_message: "{}", status_code: {})", errorStr, res));
    }
    try {
      return json::parse(result);
    } catch (const std::exception& e) {
      Logger()->error("Enable to parse json, received data is \n{}", result);
      return json::parse(fmt::format(
          R"({{ "status_message": "json parse error: {}", "status_code": {}}})",
          e.what(), -1));
    } catch (...) {
      Logger()->error("Enable to parse json, received data is \n{}", result);
      return json::parse(fmt::format(
          R"({{ "status_message": "not an std::exception", "status_code": {}}})",
          -2));
    }
  });
}

void Curl::escapeString(std::string& str) const {
  char* escaped = curl_easy_escape(_curl, str.c_str(), str.size());
  str = escaped;
  curl_free(escaped);
}

std::string Curl::escapeString(const std::string& str) const {
  char* escaped = curl_easy_escape(_curl, str.c_str(), str.size());
  std::string tmp{escaped};
  curl_free(escaped);
  return tmp;
}

} // namespace Api

} // namespace TitleFinder
