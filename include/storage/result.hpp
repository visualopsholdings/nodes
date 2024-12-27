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

#include <boost/json.hpp>
#include <memory>

using namespace std;
using json = boost::json::value;

namespace nodes {

template <typename RowType>
class Result {

public:
  
  // return the value of the query as a JSON object.
  optional<RowType> value() {
    auto j = _impl->value();
    if (!j) {
      return {};
    }
    return RowType(j.value());
  }
    
  // return the value of the query as a JSON array.
  optional<vector<RowType >> values() {
    auto j = _impl->values();
    if (!j) {
      return {};
    }
    vector<RowType > ret;
    for (auto i: j.value()) {
      ret.push_back(i);
    }
    return ret;
  }

  Result(shared_ptr<ResultImpl> impl): _impl(impl) {};

private:

  shared_ptr<ResultImpl> _impl;
  
};

} // nodes

#endif // H_result
