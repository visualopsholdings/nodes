/*
  group.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/schema.hpp"

#include "storage.hpp"

#include "log.hpp"

bool Group::getMemberSet(const string &group, set<string> *mset) {

  auto doc = findById(group, { "members" }).value();
  if (!doc) {
    L_ERROR("no group " + group);
    return false;
  }
  auto members = doc.value().j().at("members").as_array();
  
//  L_TRACE("old members " << members);

  for (auto i: members) {
    mset->insert(i.at("user").as_string().c_str());
  }
  
//  L_TRACE("new set " << boost::algorithm::join(mset, ", "));

  return true;
  
}

bool Group::saveMemberSet(const string &group, const set<string> &mset) {

  boost::json::array newmembers;
  for (auto i: mset) {
    newmembers.push_back(json{
      { "user", i }
    });
  }
  
  auto result = Group().updateById(group, {
    { "modifyDate", Storage::instance()->getNow() },
    { "members", newmembers }
  });
  if (!result) {
    L_ERROR("can't update group " + group);
    return false;
  }
  
  return true;

}

bool Group::addMember(const string &group, const string &user) {

  set<string> mset;
  if (getMemberSet(group, &mset)) {
    mset.insert(user);
    return saveMemberSet(group, mset);
  }
  
  return false;

}

bool Group::removeMember(const string &group, const string &user) {

  set<string> mset;
  if (getMemberSet(group, &mset)) {
    mset.erase(user);
    return saveMemberSet(group, mset);
  }
  
  return false;
  
}
