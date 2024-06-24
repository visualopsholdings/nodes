/*
  testsecurity.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "security.hpp"

#include "vid.hpp"

#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( PiVID )
{
  cout << "=== PiVID ===" << endl;
  
  // these would be stored with the User in teh DB.
  string salt = "8BT0BEzcAQxE1ZtYcAIhHdN1L62xmSraWwwQdErnJgLb3iprl0yM1itsirteYRS5mnmTJT+qybk9PaLdIOJ7SQXv7+I0r6XzlM6o/G9HYw8tf9tRulECVQ0FvgfDHt1ZEzXEukeptsOJD/PfE7N2MPWDVgj55xkgb5kZ4F9eGkc=";
  string hash = "8zkMIA1llK50OpBLvXDIiDd1L2A8b1rMQZOnpn/ghHZNamhfR6pXLGWoEID6Kckw8nn6/uszpASZcKy2nuJGq3pe5J8WRpdNQ74D2m0wwT0VtXZzdox5JCM0xwCoZ4zlTCXqXqZn2MArieoAUNmMGPO31o8KZM49ICWbtTNhmcI=";
  
  // and this is passed in from the user.
  VID vid("Vk9WNIdltNaXa0eOG9cAdmlzdWFsb3Bz");
  
  BOOST_CHECK(Security::valid(vid, salt, hash));
  
}
