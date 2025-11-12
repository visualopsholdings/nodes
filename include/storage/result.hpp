/*
  result.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB result for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_result
#define H_result

#include "storage/resulti.hpp"

#include <memory>

using namespace std;

namespace nodes {

template <typename RowType>
class Result {

public:
  
  // return the value of the query as an object.
  optional<RowType> one() {
    auto v = _impl->value();
    if (!v) {
      return {};
    }
    return RowType(v.value());
  }
    
  // return the value of the query as an array.
  optional<vector<RowType> > all() {
    auto v = _impl->all();
    if (!v) {
      return {};
    }
    vector<RowType> ret;
    for (auto i: v.value()) {
      auto obj = vops::Dict::getObject(i);
      if (!obj) {
//        cerr << "obj not an object!" << endl;
        continue;
      }
      ret.push_back(RowType(*obj));
    }
    return ret;
  }

  Result(shared_ptr<ResultImpl> impl): _impl(impl) {};

private:

  shared_ptr<ResultImpl> _impl;
  
};

} // nodes

#endif // H_result
