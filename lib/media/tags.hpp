/**
 * @file media/tags.hpp
 *
 * @brief
 *
 * @author Jordan Bieder
 *
 * @copyright Copyright (C) 2023 Jordan Bieder
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

#include <map>
#include <string_view>

namespace TitleFinder {

namespace Media {

namespace Tag {

constexpr const char* const tags[] = {
    // * means already read by kodi
    //@see https://kodi.wiki/view/Video_file_tagging#Supported_Tags
    "album",       // *album
    "artist",      // *artist
    "composer",    // *writing credits (split at slashes)
    "date",        // *year
    "director",    // director
    "description", // *plot outline (overview)
    "genre",       // *genre (split at slashes)
    "location",    // Country
    "network",     // studio
    "publisher",   // studio
    "performer",   // director
    "rating",      // rating
    "sort_name",   // sort title
    "synopsis",    // *plot
    "title",       // *title
    "year",        // premiered
    nullptr};

constexpr int numberOfTags = sizeof(tags) / sizeof(const char* const) - 1;

constexpr int operator""_tagid(const char* tag, unsigned long) {
  const char* const* t = tags;
  for (int i = 0; *t; ++t, ++i) {
    const char* in = tag;
    const char* lo = *t;
    while (*in == *lo) {
      if (*in == '\0')
        break;
      in++;
      lo++;
    }
    if (*in == *lo)
      return i;
  }
  return -1;
};

using TagDict = std::map<const int, std::string>;

} // namespace Tag

} // namespace Media

} // namespace TitleFinder
