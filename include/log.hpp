/*
  log.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Dec-2024
    
  Logging for nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_log
#define H_log

#include <string>

using namespace std;

class Log {

public:
  static void config(const string &name, const string &level);
  
};

#define BOOST_LOGGING
/* 
  To use Node logging, inside CMakeLists.txt you can set:
  
  find_package(Boost 1.85.0 COMPONENTS program_options filesystem unit_test_framework chrono thread log_setup log json REQUIRED)
  ...
  set(BOOSTLIBS ${Boost_LOG_LIBRARY} ${Boost_JSON_LIBRARY})

*/

//#define STD_LOGGING
/*
  At a bare minimum, you could just cout with everything.
*/

#ifdef BOOST_LOGGING

#include <boost/log/trivial.hpp>

#define L_TRACE(_msg_)     BOOST_LOG_TRIVIAL(trace) << _msg_
#define L_DEBUG(_msg_)     BOOST_LOG_TRIVIAL(debug) << _msg_
#define L_ERROR(_msg_)     BOOST_LOG_TRIVIAL(error) << _msg_
#define L_WARNING(_msg_)   BOOST_LOG_TRIVIAL(warning) << _msg_
#define L_INFO(_msg_)      BOOST_LOG_TRIVIAL(info) << _msg_

#define HAS_BOOST_LOGGING

#endif //BOOST_LOGGING

#ifdef STD_LOGGING

#include <iostream>

#define L_TRACE(_msg_)
#define L_DEBUG(_msg_)
#define L_ERROR(_msg_)      cerr << _msg_ << std::endl
#define L_WARNING(_msg_)    cerr << _msg_ << std::endl
#define L_INFO(_msg_)       cout << _msg_ << std::endl

#endif // STD_LOGGING

#if !defined(BOOST_LOGGING) && !defined(STD_LOGGING)

/* 
  Or just do nothing at all
*/

#define L_TRACE(_msg_)
#define L_DEBUG(_msg_)
#define L_ERROR(_msg_)
#define L_WARNING(_msg_)
#define L_INFO(_msg_)  

#endif

#endif // H_log
