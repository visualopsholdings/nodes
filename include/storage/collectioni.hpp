/*
  collectioni.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  Collection implemementation for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_collectioni
#define H_collectioni

#include <mongocxx/collection.hpp>

using namespace std;

class CollectionImpl {

public:
  
  CollectionImpl(mongocxx::collection c): _c(c) {};
  CollectionImpl() {};

  mongocxx::collection _c;
  
};

#endif // H_collectioni
