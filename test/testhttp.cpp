/*
  testhttp.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include <restinio/core.hpp>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( simple )
{
  cout << "=== simple ===" << endl;
  
  restinio::run(
    restinio::on_this_thread()
      .port(8080)
//      .address("localhost")
      .address("52.62.155.91")
      .request_handler([](auto req) {
        return req->create_response().set_body("Hello, World!").done();
      }));

}
