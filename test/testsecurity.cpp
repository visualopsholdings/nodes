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
using namespace nodes;

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

}

string pisalt = "8BT0BEzcAQxE1ZtYcAIhHdN1L62xmSraWwwQdErnJgLb3iprl0yM1itsirteYRS5mnmTJT+qybk9PaLdIOJ7SQXv7+I0r6XzlM6o/G9HYw8tf9tRulECVQ0FvgfDHt1ZEzXEukeptsOJD/PfE7N2MPWDVgj55xkgb5kZ4F9eGkc=";
string pivid = "Vk9WNIdltNaXa0eOG9cAdmlzdWFsb3Bz";
string tracy = "667d0baedfb1ed18430d8ed3";
string tracysalt = "xNeZ0SvhuzyUOwbqNKStHQr7Q58gRf77YBV0fo89ARxc/SjMkAIcL/1FAPEQ2gTSBHGeSKooq3hZhNhgNRXqYn9r4WNKncasIFIj2h4bQJ6m4zTUqYXfGLA/Q9nSPWWTO037Oj0fsST5CzHx0oEfgwDehTkifcBcfKBLoKjbxO4=";
string tracyvid = "Vk9mF3iET2RYB0cHwToBZj0zxZyZMTFkaMXG+0kD2P5rVH3YdJpK";
string policy = "667bfee4b07cc40ec3dd6ee8";
string leanne = "667d0baedfb1ed18430d8ed4";
string team1 = "667d0bae39ae84d0890a2141";
string collection1 = "6622129f207af2b4e65ab90f";


BOOST_AUTO_TEST_CASE( PiVID )
{
  cout << "=== PiVID ===" << endl;
  
  dbSetup();
  
  // these would be stored with the User in the DB.
  string hash = "8zkMIA1llK50OpBLvXDIiDd1L2A8b1rMQZOnpn/ghHZNamhfR6pXLGWoEID6Kckw8nn6/uszpASZcKy2nuJGq3pe5J8WRpdNQ74D2m0wwT0VtXZzdox5JCM0xwCoZ4zlTCXqXqZn2MArieoAUNmMGPO31o8KZM49ICWbtTNhmcI=";
  
  // and this is passed in from the user.
  VID vid(pivid);
  vid.describe();
  
  BOOST_CHECK(Security::instance()->valid(vid, pisalt, hash));
  
}

BOOST_AUTO_TEST_CASE( TracyVID )
{
  cout << "=== TracyVID ===" << endl;
  
  // these would be stored with the User in the DB.
  string hash = "alRZUqzBRXT7CPNAmNPnWGoB7fvkVpW7AuagThZevCiXk4FwgPCuv6MhHr2ENDaTUBPlusi++R50dRSMAsyMBby4Cc33VvEY6SW0Pv3/EesROXedqqUKYbXRmpKoLKAb6bB5fuBx4UgDvs6/fX6D0IiCdNGgyhXurIk8roYd8Do=";
  
  // and this is passed in from the user.
  VID vid("Vk9mF3iET2RYB0cHwToBZj0zxZyZMTFkaMXG+0kD2P5rVH3YdJpK");
  vid.describe();
  
  BOOST_CHECK(Security::instance()->valid(vid, tracysalt, hash));
  
}

BOOST_AUTO_TEST_CASE( newVID )
{
  cout << "=== newVID ===" << endl;
  
  string password = Security::instance()->newPassword();
  
  VID vid;
  vid.reset(tracy, password);
  vid.describe();
  cout << vid.value() << endl;
  
  VID vid2(vid.value());
  vid2.describe();
  BOOST_CHECK(vid == vid2);
  
}

  
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

BOOST_AUTO_TEST_CASE( getPolicyGroups )
{
  cout << "=== getPolicyGroups ===" << endl;
  
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
  
  vector<string> groups;
  Security::instance()->getPolicyGroups(policy, &groups);
  BOOST_CHECK_EQUAL(groups.size(), 1);
  
}

BOOST_AUTO_TEST_CASE( with )
{
  cout << "=== with ===" << endl;
  
  dbSetup();
  Policy().deleteMany({{}});
  Group().deleteMany({{}});
  SchemaImpl::deleteManyGeneral("collections", {{}});
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
  BOOST_CHECK(SchemaImpl::insertGeneral("collections", {
    { "name", "Collection 1" },
    { "policy", policy }
  }));

  Security::instance()->regenerateGroups();
  Security::instance()->regenerate();

  {
    // tracy is in the team that can view.
    auto doc = Security::instance()->withView("collections", tracy, {{ "name", "Collection 1" }}).one();
    BOOST_CHECK(doc);
  }
  
  {
    // leanne can only edit.
    auto docs = Security::instance()->withView("collections", leanne, {{ "name", "Collection 1" }}).all();
    BOOST_CHECK(!docs);
  }
  
  {
    // leanne is in the team that can edit.
    auto docs = Security::instance()->withEdit("collections", leanne, {{ "name", "Collection 1" }}).all();
    BOOST_CHECK(docs);
    BOOST_CHECK_EQUAL(docs.value().size(), 1);
  }
  
  {
    // tracy can only view.
    auto docs = Security::instance()->withEdit("collections", tracy, {{ "name", "Collection 1" }}).all();
    BOOST_CHECK(!docs);
  }

  {
    // admin can see them all.
    auto docs = Security::instance()->withView("collections", nullopt, {{ "name", "Collection 1" }}).all();
    BOOST_CHECK(docs);
    BOOST_CHECK_EQUAL(docs.value().size(), 1);
  }
  
  {
    // even tracy can't see a collection not there.
    auto docs = Security::instance()->withView("collections", tracy, {{ "name", "Collection 2" }}).all();
    BOOST_CHECK(!docs);
  }
  
}

BOOST_AUTO_TEST_CASE( canEdit )
{
  cout << "=== canEdit ===" << endl;
  
  dbSetup();
  Policy().deleteMany({{}});
  SchemaImpl::deleteManyGeneral("collections", {{}});
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
  auto collection = SchemaImpl::insertGeneral("collections", {
    { "name", "Collection 1" },
    { "policy", policy.value() }
  });
  Security::instance()->regenerate();
  
  BOOST_CHECK(Security::instance()->canEdit("collections", tracy, collection.value()));
  BOOST_CHECK(!Security::instance()->canEdit("collections", leanne, collection.value()));
  BOOST_CHECK(!Security::instance()->canEdit("collections", "$tracy", collection.value()));
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

BOOST_AUTO_TEST_CASE( modifyPolicy )
{
  cout << "=== modifyPolicy ===" << endl;
  
  dbSetup();
  User().deleteMany({{}});
  Policy().deleteMany({{}});
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
  boost::json::array empty;
  BOOST_CHECK(Policy().insert({
    { "_id", { { "$oid", policy } } },
    { "accesses", {
      { { "name", "view" }, 
        { "groups", empty },
        { "users", { tracy } }
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

  // add and remove a bunch of stuff.
  vector<tuple<string, string, string > > add;
  add.push_back({ "view", "user", leanne });
  add.push_back({ "exec", "group", team1 });
  vector<string> remove = { "//accesses/2/users/0" };
  auto newpolicy =  Security::instance()->modifyPolicy(policy, add, remove);
  BOOST_CHECK(newpolicy);
  BOOST_CHECK(newpolicy.value() != policy);

  auto policy = Policy().findById(newpolicy.value()).one();
  BOOST_CHECK(policy);
  BOOST_CHECK_EQUAL(policy.value().accesses().size(), 3);
  BOOST_CHECK_EQUAL(policy.value().accesses()[0].users().size(), 2); // 1 added
  BOOST_CHECK_EQUAL(policy.value().accesses()[0].groups().size(), 0); // didn't change
  BOOST_CHECK_EQUAL(policy.value().accesses()[1].users().size(), 0); // didn't change
  BOOST_CHECK_EQUAL(policy.value().accesses()[1].groups().size(), 1); // didn't change
  BOOST_CHECK_EQUAL(policy.value().accesses()[2].users().size(), 1); // 1 removed
  BOOST_CHECK_EQUAL(policy.value().accesses()[2].groups().size(), 1); // 1 added
  
}

BOOST_AUTO_TEST_CASE( generateShareLink )
{
  cout << "=== generateShareLink ===" << endl;
  
  dbSetup();
  User().deleteMany({{}});
  Group().deleteMany({{}});
  SchemaImpl::deleteManyGeneral("collections", {{}});
  Info().deleteMany({{}});
  BOOST_CHECK(Info().insert({
    { "type", "tokenKey" },
    { "text", "90B21444AC1A2C9146FFA34C72BF787F76E7B0EDD236F0508571264153E58A787B82729268EF67DFCCC6B1F113721B0D752DA65DA6BC82BCA9A1C73E58DAAFF7" }
  }));
  BOOST_CHECK(Info().insert({
    { "type", "tokenIV" },
    { "text", "D764E7CAE16C361A4546873B" }
  }));
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", tracy } } },
    { "name", "tracy" },
    { "fullname", "Tracy" }
  }));
  BOOST_CHECK(Group().insert({
    { "_id", { { "$oid", team1 } } },
    { "name", "Team 1" },
    { "members", {
      { { "user", tracy } },
      } 
    }
  }));
  BOOST_CHECK(SchemaImpl::insertGeneral("collections", {
    { "_id", { { "$oid", collection1 } } },
    { "name", "Collection 1" },
    { "bits", 2048 }
  }));
  
  auto link = Security::instance()->generateShareLink(tracy, "http://visualops.com/apps/chat/#/streams/" + collection1, "collections", collection1, team1, 4, "bits");
  BOOST_CHECK(link);
  BOOST_CHECK_EQUAL(link.value().find("token="), 66);
  
}

BOOST_AUTO_TEST_CASE( streamShareToken )
{
  cout << "=== streamShareToken ===" << endl;
  
  dbSetup();
  Info().deleteMany({{}});
  BOOST_CHECK(Info().insert({
    { "type", "tokenKey" },
    { "text", "90B21444AC1A2C9146FFA34C72BF787F76E7B0EDD236F0508571264153E58A787B82729268EF67DFCCC6B1F113721B0D752DA65DA6BC82BCA9A1C73E58DAAFF7" }
  }));
  BOOST_CHECK(Info().insert({
    { "type", "tokenIV" },
    { "text", "D764E7CAE16C361A4546873B" }
  }));
  
  string options = "mustName";
  string expires = "2024-09-13T11:56:24.0+00:00";
  auto token = Security::instance()->createShareToken(collection1, tracy, options, team1, expires);
  BOOST_CHECK(token);
  cout << token.value() << endl;
  auto json = Security::instance()->expandShareToken(token.value());
  BOOST_CHECK(json);
  cout << json.value() << endl;
  BOOST_CHECK(json.value().is_object());
  auto obj = json.value().as_object();
  BOOST_CHECK(obj.if_contains("id"));
  BOOST_CHECK_EQUAL(obj.at("id").as_string(), collection1);
  BOOST_CHECK(obj.if_contains("user"));
  BOOST_CHECK_EQUAL(obj.at("user").as_string(), tracy);
  BOOST_CHECK(obj.if_contains("options"));
  BOOST_CHECK_EQUAL(obj.at("options").as_string(), options);
  BOOST_CHECK(obj.if_contains("team"));
  BOOST_CHECK_EQUAL(obj.at("team").as_string(), team1);
  BOOST_CHECK(obj.if_contains("expires"));
  BOOST_CHECK_EQUAL(obj.at("expires").as_string(), expires);
  
}

BOOST_AUTO_TEST_CASE( newSalt )
{
  cout << "=== newSalt ===" << endl;
  
  dbSetup();
  string salt = Security::instance()->newSalt();
//  cout << salt << endl;
  BOOST_CHECK_EQUAL(salt.size(), 172);
  
}

BOOST_AUTO_TEST_CASE( newHash )
{
  cout << "=== newHash ===" << endl;
  
  dbSetup();
  VID vid(tracyvid);
  cout << vid.password() << endl;
  cout << tracysalt << endl;
  string hash = Security::instance()->newHash(vid.password(), tracysalt);
  cout << hash << endl;
  BOOST_CHECK_EQUAL(hash.size(), 172);
  BOOST_CHECK(Security::instance()->valid(vid, tracysalt, hash));
  
}
