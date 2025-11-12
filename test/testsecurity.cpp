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
#include "dict.hpp"

#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;
using namespace vops;

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
  Policy().deleteMany(dictO({}));
  DictV empty;
  BOOST_CHECK(Policy().insert(dictO({
    { "_id", dictO({ { "$oid", policy } }) },
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", empty },
        { "users", DictV{ "u1", "u2" } }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", empty },
        { "users",  DictV{ "u1", "u2" } }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", empty },
        { "users",  DictV{ "u1", "u2" } }
        })
      }
    }
  })));

  vector<string> users;
  Security::instance()->getPolicyUsers(policy, &users);
  BOOST_CHECK_EQUAL(users.size(), 2);
  
}

BOOST_AUTO_TEST_CASE( getPolicyUsersInGroup )
{
  cout << "=== getPolicyUsersInGroup ===" << endl;
  
  dbSetup();
  Policy().deleteMany(dictO({}));
  Group().deleteMany(dictO({}));
  BOOST_CHECK(Group().insert(dictO({
    { "_id", dictO({ { "$oid", team1 } }) },
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", tracy } }), // tracy
      dictO({ { "user", leanne } })  // leanne
      } 
    }
  })));
  DictV empty;
  BOOST_CHECK(Policy().insert(dictO({
    { "_id", dictO({ { "$oid", policy } }) },
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        })
      } 
    }
  })));
  
  vector<string> users;
  Security::instance()->getPolicyUsers(policy, &users);
  BOOST_CHECK_EQUAL(users.size(), 2);
  
}

BOOST_AUTO_TEST_CASE( getPolicyGroups )
{
  cout << "=== getPolicyGroups ===" << endl;
  
  dbSetup();
  Policy().deleteMany(dictO({}));
  Group().deleteMany(dictO({}));
  BOOST_CHECK(Group().insert(dictO({
    { "_id", dictO({ { "$oid", team1 } }) },
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", tracy } }), // tracy
      dictO({ { "user", leanne } })  // leanne
      } 
    }
  })));
  DictV empty;
  BOOST_CHECK(Policy().insert(dictO({
    { "_id", dictO({ { "$oid", policy } }) },
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        })
      } 
    }
  })));
  
  vector<string> groups;
  Security::instance()->getPolicyGroups(policy, &groups);
  BOOST_CHECK_EQUAL(groups.size(), 1);
  
}

BOOST_AUTO_TEST_CASE( with )
{
  cout << "=== with ===" << endl;
  
  dbSetup();
  Policy().deleteMany(dictO({}));
  Group().deleteMany(dictO({}));
  SchemaImpl::deleteManyGeneral("collections", dictO({}));
  BOOST_CHECK(Group().insert(dictO({
    { "_id", dictO({ { "$oid", team1 } }) },
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", tracy } }), // tracy
      } 
    }
  })));
  DictV empty;
  BOOST_CHECK(Policy().insert(dictO({
    { "_id", dictO({ { "$oid", policy } }) },
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", DictV{ team1 } },
        { "users", empty }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", empty },
        { "users", DictV{ leanne } } // leanne
        }),
      dictO({ { "name", "exec" }, 
        { "groups", empty },
        { "users", empty }
        })
      } 
    }
  })));
  BOOST_CHECK(SchemaImpl::insertGeneral("collections", dictO({
    { "name", "Collection 1" },
    { "policy", policy }
  })));

  Security::instance()->regenerateGroups();
  Security::instance()->regenerate();

  {
    // tracy is in the team that can view.
    auto doc = Security::instance()->withView("collections", tracy, dictO({{ "name", "Collection 1" }})).one();
    BOOST_CHECK(doc);
  }
  
  {
    // leanne can only edit.
    auto docs = Security::instance()->withView("collections", leanne, dictO({{ "name", "Collection 1" }})).all();
    BOOST_CHECK(!docs);
  }
  
  {
    // leanne is in the team that can edit.
    auto docs = Security::instance()->withEdit("collections", leanne, dictO({{ "name", "Collection 1" }})).all();
    BOOST_CHECK(docs);
    BOOST_CHECK_EQUAL(docs.value().size(), 1);
  }
  
  {
    // tracy can only view.
    auto docs = Security::instance()->withEdit("collections", tracy, dictO({{ "name", "Collection 1" }})).all();
    BOOST_CHECK(!docs);
  }

  {
    // admin can see them all.
    auto docs = Security::instance()->withView("collections", nullopt, dictO({{ "name", "Collection 1" }})).all();
    BOOST_CHECK(docs);
    BOOST_CHECK_EQUAL(docs.value().size(), 1);
  }
  
  {
    // even tracy can't see a collection not there.
    auto docs = Security::instance()->withView("collections", tracy, dictO({{ "name", "Collection 2" }})).all();
    BOOST_CHECK(!docs);
  }
  
}

BOOST_AUTO_TEST_CASE( canEdit )
{
  cout << "=== canEdit ===" << endl;
  
  dbSetup();
  Policy().deleteMany(dictO({}));
  SchemaImpl::deleteManyGeneral("collections", dictO({}));
  DictV empty;
  auto policy = Policy().insert(dictO({
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", empty },
        { "users", DictV{ tracy, leanne } }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", empty },
        { "users", DictV{ tracy } }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", empty },
        { "users", empty }
        })
      } 
    }
  }));
  auto collection = SchemaImpl::insertGeneral("collections", dictO({
    { "name", "Collection 1" },
    { "policy", policy.value() }
  }));
  Security::instance()->regenerate();
  
  BOOST_CHECK(Security::instance()->canEdit("collections", tracy, collection.value()));
  BOOST_CHECK(!Security::instance()->canEdit("collections", leanne, collection.value()));
  BOOST_CHECK(!Security::instance()->canEdit("collections", "$tracy", collection.value()));
}

BOOST_AUTO_TEST_CASE( getPolicyLines )
{
  cout << "=== getPolicyLines ===" << endl;
  
  dbSetup();
  User().deleteMany(dictO({}));
  Group().deleteMany(dictO({}));
  Policy().deleteMany(dictO({}));
  DictV empty;
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({ { "$oid", tracy } }) },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  })));
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({ { "$oid", leanne } }) },
    { "name", "leanne" },
    { "admin", false },
    { "fullname", "Leanne" }
  })));
  BOOST_CHECK(Group().insert(dictO({
    { "_id", dictO({ { "$oid", team1 } }) },
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", tracy } }), // tracy
      } 
    }
  })));
  BOOST_CHECK(Policy().insert(dictO({
    { "_id", dictO({ { "$oid", policy } }) },
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", empty },
        { "users", DictV{ tracy, leanne } }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", DictV{ team1 } },
        { "users",  empty }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", empty },
        { "users",  DictV{ tracy, leanne } }
        })
      } 
    }
  })));

  auto lines =  Security::instance()->getPolicyLines(policy);
  BOOST_CHECK(lines);
//  cout << Dict::toString(lines.value()) << endl;
  BOOST_CHECK_EQUAL(lines->size(), 5);

  auto user = (*lines)[0];
  BOOST_CHECK_EQUAL(*Dict::getStringG(user, "path"), "//accesses/0/users/0");
  BOOST_CHECK_EQUAL(*Dict::getStringG(user, "type"), "view");
  BOOST_CHECK_EQUAL(*Dict::getStringG(user, "context"), "user");
  BOOST_CHECK_EQUAL(*Dict::getStringG(user, "name"), "Tracy");

  auto group = (*lines)[2];
  BOOST_CHECK_EQUAL(*Dict::getStringG(group, "path"), "//accesses/1/groups/0");
  BOOST_CHECK_EQUAL(*Dict::getStringG(group, "type"), "edit");
  BOOST_CHECK_EQUAL(*Dict::getStringG(group, "context"), "group");
  BOOST_CHECK_EQUAL(*Dict::getStringG(group, "name"), "Team 1");
  
}

BOOST_AUTO_TEST_CASE( findMissingPolicyForUser )
{
  cout << "=== findMissingPolicyForUser ===" << endl;
  
  dbSetup();
  User().deleteMany(dictO({}));
  Policy().deleteMany(dictO({}));
  auto user = User().insert(dictO({
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  }));
  BOOST_CHECK(user);

  auto foundpolicy =  Security::instance()->findPolicyForUser(user.value());
  BOOST_CHECK(foundpolicy);

}

BOOST_AUTO_TEST_CASE( findPolicyForUser )
{
  cout << "=== findPolicyForUser ===" << endl;
  
  dbSetup();
  User().deleteMany(dictO({}));
  Policy().deleteMany(dictO({}));
  auto user = User().insert(dictO({
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  }));
  BOOST_CHECK(user);
  DictV empty;
  auto policy = Policy().insert(dictO({
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", empty },
        { "users", DictV{ user.value() } }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", empty },
        { "users", DictV{ user.value() } }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", empty },
        { "users", DictV{ user.value() } }
        })
      } 
    }
  }));
  BOOST_CHECK(policy);

  auto foundpolicy =  Security::instance()->findPolicyForUser(user.value());
  BOOST_CHECK(foundpolicy);
  BOOST_CHECK_EQUAL(foundpolicy.value(), policy.value());

}

BOOST_AUTO_TEST_CASE( modifyPolicy )
{
  cout << "=== modifyPolicy ===" << endl;
  
  dbSetup();
  User().deleteMany(dictO({}));
  Policy().deleteMany(dictO({}));
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({ { "$oid", tracy } }) },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  })));
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({ { "$oid", leanne } }) },
    { "name", "leanne" },
    { "admin", false },
    { "fullname", "Leanne" }
  })));
  BOOST_CHECK(Group().insert(dictO({
    { "_id", dictO({ { "$oid", team1 } }) },
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", tracy } }), // tracy
      } 
    }
  })));
  DictV empty;
  BOOST_CHECK(Policy().insert(dictO({
    { "_id", dictO({ { "$oid", policy } }) },
    { "accesses", DictV{
      dictO({ { "name", "view" }, 
        { "groups", empty },
        { "users", DictV{ tracy } }
        }),
      dictO({ { "name", "edit" }, 
        { "groups", DictV{ team1 } },
        { "users",  empty }
        }),
      dictO({ { "name", "exec" }, 
        { "groups", empty },
        { "users", DictV{ tracy, leanne } }
        })
      } 
    }
  })));

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
  User().deleteMany(dictO({}));
  Group().deleteMany(dictO({}));
  SchemaImpl::deleteManyGeneral("collections", dictO({}));
  Info().deleteMany(dictO({}));
  BOOST_CHECK(Info().insert(dictO({
    { "type", "tokenKey" },
    { "text", "90B21444AC1A2C9146FFA34C72BF787F76E7B0EDD236F0508571264153E58A787B82729268EF67DFCCC6B1F113721B0D752DA65DA6BC82BCA9A1C73E58DAAFF7" }
  })));
  BOOST_CHECK(Info().insert(dictO({
    { "type", "tokenIV" },
    { "text", "D764E7CAE16C361A4546873B" }
  })));
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({ { "$oid", tracy } }) },
    { "name", "tracy" },
    { "fullname", "Tracy" }
  })));
  BOOST_CHECK(Group().insert(dictO({
    { "_id", dictO({ { "$oid", team1 } }) },
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", tracy } }),
      } 
    }
  })));
  BOOST_CHECK(SchemaImpl::insertGeneral("collections", dictO({
    { "_id", dictO({ { "$oid", collection1 } }) },
    { "name", "Collection 1" },
    { "bits", 2048 }
  })));
  
  auto link = Security::instance()->generateShareLink(tracy, "http://visualops.com/apps/chat/#/streams/" + collection1, "collections", collection1, team1, 4, "bits");
  BOOST_CHECK(link);
  BOOST_CHECK_EQUAL(link.value().find("token="), 66);
  
}

BOOST_AUTO_TEST_CASE( streamShareToken )
{
  cout << "=== streamShareToken ===" << endl;
  
  dbSetup();
  Info().deleteMany(dictO({}));
  BOOST_CHECK(Info().insert(dictO({
    { "type", "tokenKey" },
    { "text", "90B21444AC1A2C9146FFA34C72BF787F76E7B0EDD236F0508571264153E58A787B82729268EF67DFCCC6B1F113721B0D752DA65DA6BC82BCA9A1C73E58DAAFF7" }
  })));
  BOOST_CHECK(Info().insert(dictO({
    { "type", "tokenIV" },
    { "text", "D764E7CAE16C361A4546873B" }
  })));
  
  string options = "mustName";
  string expires = "2024-09-13T11:56:24.0+00:00";
  auto token = Security::instance()->createShareToken(collection1, tracy, options, team1, expires);
  BOOST_CHECK(token);
  cout << token.value() << endl;
  auto json = Security::instance()->expandShareToken(token.value());
  BOOST_CHECK(json);
  cout << Dict::toString(json.value()) << endl;
  BOOST_CHECK_EQUAL(*Dict::getString(json.value(), "id"), collection1);
  BOOST_CHECK_EQUAL(*Dict::getString(json.value(), "user"), tracy);
  BOOST_CHECK_EQUAL(*Dict::getString(json.value(), "options"), options);
  BOOST_CHECK_EQUAL(*Dict::getString(json.value(), "team"), team1);
  BOOST_CHECK_EQUAL(*Dict::getString(json.value(), "expires"), expires);
  
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
