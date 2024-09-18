/*
  date.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Sep-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "date.hpp"

#include <sstream>
#include <ctime>
#include <iomanip>
#include <boost/log/trivial.hpp>
#include <cstdlib>

long Date::now() {

  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) * 1000;

}

string Date::toISODate(long t) {

  time_t tnum = t / 1000;
  int ms = t - (tnum * 1000);

  tm tm = *gmtime(&tnum);
  stringstream ss;
  ss << put_time(&tm, "%FT%T");
  ss << ".";
  ss << ms;
  ss << "+00:00";

  return ss.str();
  
}

long Date::fromISODate(const string &d) {

  auto dot = d.rfind(".");
  string start = d.substr(0, dot);
//  BOOST_LOG_TRIVIAL(trace) << start;
  
  int ms = atoi(d.substr(dot+1, 3).c_str());
//  BOOST_LOG_TRIVIAL(trace) << ms;

  tm tm = {};
  istringstream ss(start);
  ss >> get_time(&tm, "%FT%T");
   
  return (timegm(&tm) * 1000) + ms;
  
}
