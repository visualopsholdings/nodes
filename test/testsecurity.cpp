/*
  testsecurity.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "security.hpp"

#include "vid.hpp"
#include "storage.hpp"

#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

}

BOOST_AUTO_TEST_CASE( PiVID )
{
  cout << "=== PiVID ===" << endl;
  
  // these would be stored with the User in the DB.
  string salt = "8BT0BEzcAQxE1ZtYcAIhHdN1L62xmSraWwwQdErnJgLb3iprl0yM1itsirteYRS5mnmTJT+qybk9PaLdIOJ7SQXv7+I0r6XzlM6o/G9HYw8tf9tRulECVQ0FvgfDHt1ZEzXEukeptsOJD/PfE7N2MPWDVgj55xkgb5kZ4F9eGkc=";
  string hash = "8zkMIA1llK50OpBLvXDIiDd1L2A8b1rMQZOnpn/ghHZNamhfR6pXLGWoEID6Kckw8nn6/uszpASZcKy2nuJGq3pe5J8WRpdNQ74D2m0wwT0VtXZzdox5JCM0xwCoZ4zlTCXqXqZn2MArieoAUNmMGPO31o8KZM49ICWbtTNhmcI=";
  
  // and this is passed in from the user.
  VID vid("Vk9WNIdltNaXa0eOG9cAdmlzdWFsb3Bz");
  vid.describe();
  
  BOOST_CHECK(Security::instance()->valid(vid, salt, hash));
  
}

BOOST_AUTO_TEST_CASE( TracyVID )
{
  cout << "=== TracyVID ===" << endl;
  
  // these would be stored with the User in the DB.
  string salt = "xNeZ0SvhuzyUOwbqNKStHQr7Q58gRf77YBV0fo89ARxc/SjMkAIcL/1FAPEQ2gTSBHGeSKooq3hZhNhgNRXqYn9r4WNKncasIFIj2h4bQJ6m4zTUqYXfGLA/Q9nSPWWTO037Oj0fsST5CzHx0oEfgwDehTkifcBcfKBLoKjbxO4=";
  string hash = "alRZUqzBRXT7CPNAmNPnWGoB7fvkVpW7AuagThZevCiXk4FwgPCuv6MhHr2ENDaTUBPlusi++R50dRSMAsyMBby4Cc33VvEY6SW0Pv3/EesROXedqqUKYbXRmpKoLKAb6bB5fuBx4UgDvs6/fX6D0IiCdNGgyhXurIk8roYd8Do=";
  
  // and this is passed in from the user.
  VID vid("Vk9mF3iET2RYB0cHwToBZj0zxZyZMTFkaMXG+0kD2P5rVH3YdJpK");
  vid.describe();
  
  BOOST_CHECK(Security::instance()->valid(vid, salt, hash));
  
}

string policy = "667bfee4b07cc40ec3dd6ee8";
string tracy = "667d0baedfb1ed18430d8ed3";
string leanne = "667d0baedfb1ed18430d8ed4";
string team1 = "667d0bae39ae84d0890a2141";
  
BOOST_AUTO_TEST_CASE( getPolicyUsers )
{
  cout << "=== getPolicyUsers ===" << endl;
  
  dbSetup();
  Policy().deleteMany({{}});
  boost::json::array empty;
  BOOST_CHECK(Policy().insert({
    { "_id", { { "$oid", policy } } },
    { "accesses", {
      { { "name", "view" }, 
        { "groups", empty },
        { "users", { "u1", "u2" } }
        },
      { { "name", "edit" }, 
        { "groups", empty },
        { "users",  { "u1", "u2" } }
        },
      { { "name", "exec" }, 
        { "groups", empty },
        { "users",  { "u1", "u2" } }
        }
      } 
    }
  }));

  vector<string> users;
  Security::instance()->getPolicyUsers(policy, &users);
  BOOST_CHECK_EQUAL(users.size(), 2);
  
}

BOOST_AUTO_TEST_CASE( getPolicyUsersInGroup )
{
  cout << "=== getPolicyUsersInGroup ===" << endl;
  
  dbSetup();
  Policy().deleteMany({{}});
  Group().deleteMany({{}});
  BOOST_CHECK(Group().insert({
    { "_id", { { "$oid", team1 } } },
    { "name", "Team 1" },
    { "members", {
      { { "user", tracy } }, // tracy
      { { "user", leanne } }  // leanne
      } 
    }
  }));
  boost::json::array empty;
  BOOST_CHECK(Policy().insert({
    { "_id", { { "$oid", policy } } },
    { "accesses", {
      { { "name", "view" }, 
        { "groups", { team1 } },
        { "users", empty }
        },
      { { "name", "edit" }, 
        { "groups", { team1 } },
        { "users", empty }
        },
      { { "name", "exec" }, 
        { "groups", { team1 } },
        { "users", empty }
        }
      } 
    }
  }));
  
  vector<string> users;
  Security::instance()->getPolicyUsers(policy, &users);
  BOOST_CHECK_EQUAL(users.size(), 2);
  
}

BOOST_AUTO_TEST_CASE( with )
{
  cout << "=== with ===" << endl;
  
  dbSetup();
  Policy().deleteMany({{}});
  Group().deleteMany({{}});
  Stream().deleteMany({{}});
  BOOST_CHECK(Group().insert({
    { "_id", { { "$oid", team1 } } },
    { "name", "Team 1" },
    { "members", {
      { { "user", tracy } }, // tracy
      } 
    }
  }));
  boost::json::array empty;
  BOOST_CHECK(Policy().insert({
    { "_id", { { "$oid", policy } } },
    { "accesses", {
      { { "name", "view" }, 
        { "groups", { team1 } },
        { "users", empty }
        },
      { { "name", "edit" }, 
        { "groups", empty },
        { "users", { leanne } } // leanne
        },
      { { "name", "exec" }, 
        { "groups", empty },
        { "users", empty }
        }
      } 
    }
  }));
  BOOST_CHECK(Stream().insert({
    { "name", "Conversation 1" },
    { "policy", policy }
  }));

  Group().aggregate("../src/useringroups.json");
  Policy().aggregate("../src/groupviewpermissions.json");
  Policy().aggregate("../src/userviewpermissions.json");
  Policy().aggregate("../src/groupeditpermissions.json");
  Policy().aggregate("../src/usereditpermissions.json");

  Stream streams;
  {
    // tracy is in the team that can view.
    auto doc = Security::instance()->withView(streams, tracy, {{ "name", "Conversation 1" }}).value();
    BOOST_CHECK(doc);
  }
  
  {
    // leanne can only edit.
    auto docs = Security::instance()->withView(streams, leanne, {{ "name", "Conversation 1" }}).values();
    BOOST_CHECK(!docs);
  }
  
  {
    // leanne is in the team that can edit.
    auto docs = Security::instance()->withEdit(streams, leanne, {{ "name", "Conversation 1" }}).values();
    BOOST_CHECK(docs);
    BOOST_CHECK_EQUAL(docs.value().size(), 1);
  }
  
  {
    // tracy can only view.
    auto docs = Security::instance()->withEdit(streams, tracy, {{ "name", "Conversation 1" }}).values();
    BOOST_CHECK(!docs);
  }

  {
    // admin can see them all.
    auto docs = Security::instance()->withView(streams, nullopt, {{ "name", "Conversation 1" }}).values();
    BOOST_CHECK(docs);
    BOOST_CHECK_EQUAL(docs.value().size(), 1);
  }
  
  {
    // even tracy can't see a conversation not there.
    auto docs = Security::instance()->withView(streams, tracy, {{ "name", "Conversation 2" }}).values();
    BOOST_CHECK(!docs);
  }
  
}

BOOST_AUTO_TEST_CASE( canEdit )
{
  cout << "=== canEdit ===" << endl;
  
  dbSetup();
  Policy().deleteMany({{}});
  Stream().deleteMany({{}});
  boost::json::array empty;
  auto policy = Policy().insert({
    { "accesses", {
      { { "name", "view" }, 
        { "groups", empty },
        { "users", { tracy, leanne } }
        },
      { { "name", "edit" }, 
        { "groups", empty },
        { "users", { tracy } }
        },
      { { "name", "exec" }, 
        { "groups", empty },
        { "users", empty }
        }
      } 
    }
  });
  auto stream = Stream().insert({
    { "name", "Conversation 1" },
    { "policy", policy.value() }
  });
  Policy().aggregate("../src/groupviewpermissions.json");
  Policy().aggregate("../src/userviewpermissions.json");
  Policy().aggregate("../src/groupeditpermissions.json");
  Policy().aggregate("../src/usereditpermissions.json");
  
  Stream streams;
  BOOST_CHECK(Security::instance()->canEdit(streams, tracy, stream.value()));
  BOOST_CHECK(!Security::instance()->canEdit(streams, leanne, stream.value()));
}

BOOST_AUTO_TEST_CASE( getPolicyLines )
{
  cout << "=== getPolicyLines ===" << endl;
  
  dbSetup();
  User().deleteMany({{}});
  Group().deleteMany({{}});
  Policy().deleteMany({{}});
  boost::json::array empty;
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", tracy } } },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  }));
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", leanne } } },
    { "name", "leanne" },
    { "admin", false },
    { "fullname", "Leanne" }
  }));
  BOOST_CHECK(Group().insert({
    { "_id", { { "$oid", team1 } } },
    { "name", "Team 1" },
    { "members", {
      { { "user", tracy } }, // tracy
      } 
    }
  }));
  BOOST_CHECK(Policy().insert({
    { "_id", { { "$oid", policy } } },
    { "accesses", {
      { { "name", "view" }, 
        { "groups", empty },
        { "users", { tracy, leanne } }
        },
      { { "name", "edit" }, 
        { "groups", { team1 } },
        { "users",  empty }
        },
      { { "name", "exec" }, 
        { "groups", empty },
        { "users",  { tracy, leanne } }
        }
      } 
    }
  }));

  auto lines =  Security::instance()->getPolicyLines(policy);
  BOOST_CHECK(lines);
//  cout << lines.value() << endl;
  BOOST_CHECK(lines.value().is_array());
  BOOST_CHECK_EQUAL(lines.value().as_array().size(), 5);

  auto user = lines.value().as_array()[0];
  BOOST_CHECK_EQUAL(user.at("path").as_string(), "//accesses/0/users/0");
  BOOST_CHECK_EQUAL(user.at("type").as_string(), "view");
  BOOST_CHECK_EQUAL(user.at("context").as_string(), "user");
  BOOST_CHECK_EQUAL(user.at("name").as_string(), "Tracy");

  auto group = lines.value().as_array()[2];
  BOOST_CHECK_EQUAL(group.at("path").as_string(), "//accesses/1/groups/0");
  BOOST_CHECK_EQUAL(group.at("type").as_string(), "edit");
  BOOST_CHECK_EQUAL(group.at("context").as_string(), "group");
  BOOST_CHECK_EQUAL(group.at("name").as_string(), "Team 1");
  
}

BOOST_AUTO_TEST_CASE( findMissingPolicyForUser )
{
  cout << "=== findMissingPolicyForUser ===" << endl;
  
  dbSetup();
  User().deleteMany({{}});
  Policy().deleteMany({{}});
  auto user = User().insert({
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  });
  BOOST_CHECK(user);

  auto foundpolicy =  Security::instance()->findPolicyForUser(user.value());
  BOOST_CHECK(foundpolicy);

}

BOOST_AUTO_TEST_CASE( findPolicyForUser )
{
  cout << "=== findPolicyForUser ===" << endl;
  
  dbSetup();
  User().deleteMany({{}});
  Policy().deleteMany({{}});
  auto user = User().insert({
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  });
  BOOST_CHECK(user);
  boost::json::array empty;
  auto policy = Policy().insert({
    { "accesses", {
      { { "name", "view" }, 
        { "groups", empty },
        { "users", { user.value() } }
        },
      { { "name", "edit" }, 
        { "groups", empty },
        { "users", { user.value() } }
        },
      { { "name", "exec" }, 
        { "groups", empty },
        { "users", { user.value() } }
        }
      } 
    }
  });
  BOOST_CHECK(policy);

  auto foundpolicy =  Security::instance()->findPolicyForUser(user.value());
  BOOST_CHECK(foundpolicy);
  BOOST_CHECK_EQUAL(foundpolicy.value(), policy.value());

}
