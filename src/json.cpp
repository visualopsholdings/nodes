/*
  json.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "json.hpp"

#include <sstream>
#include <ctime>
#include <iomanip>

string Json::toISODate(json &date) {

  if (!date.is_object()) {
    return "not_object";
  }
  
  if (!date.as_object().if_contains("$date")) {
    return "bad_object";
  }
  
  time_t ts = date.at("$date").as_int64();

  time_t tnum = ts / 1000;
  int secs = ts - (tnum * 1000);

  tm tm = *gmtime(&tnum);
  stringstream ss;
  ss << put_time(&tm, "%FT%T.");
  ss << secs;
  ss << "+00:00";

  return ss.str();
  
}
