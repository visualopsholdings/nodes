/*
  objectsmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"
#include "data.hpp"
#include "dict.hpp"

namespace nodes {

void objectsMsg(Server *server, const IncomingMsg &m) {

  DictO query{dictO({
    { "deleted", dictO({
      { "$ne", true }
      })
    }
  })};
  
  if (!m.objtype) {
    server->sendErr("Message missing objtype");
    return;
  }
  
  // add in any parent query.
  string parent;
  if (Storage::instance()->parentInfo(*m.objtype, &parent)) {
    auto pid = m.extra_fields.get(parent);
    if (!pid) {
      server->sendErr("no " + parent);
      return;
    }
    query[parent] = *pid;
  }
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(*m.objtype, &coll)) {
    server->sendErr("Could not get collection name for " + *m.objtype);
    return;
  }
  
  // make sure it can be viewed.
  auto docs = Security::instance()->withView(coll, m.me, query).all();
  
  // copy out all the data to return;
  DictV s;
  if (docs) {
    transform(docs->begin(), docs->end(), back_inserter(s), [](auto e) { 
      return e.dict(); 
    });
  }
  
  server->sendCollection(m, coll, s);

}

};