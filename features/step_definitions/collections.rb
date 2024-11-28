require 'json'

When('she sends collections as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "objects", "objtype": "collection", "me": u })
end

When('she sends collections') do
   $lastResult = Send({ "type": "objects", "objtype": "collection" })
end

Then('she receives {int} collections') do |count|
   expect($lastResult["type"]).to eq("collections")
   expect($lastResult["collections"].length).to eq(count)
end

When('she sends collection {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Collection.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "object", "objtype": "collection", "me": u, "collection": s })
end

When('she sends collection with id {string}') do |id|
   $lastResult = Send({ "type": "object", "objtype": "collection", "collection": id })
end

Then('she receives collection {string}') do |name|
   expect($lastResult["type"]).to eq("collection")
   expect($lastResult["collection"]["name"]).to eq(name)
end

When('she sends add collection {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "addobject", "objtype": "collection", "me": u, "name": name })
end

When('she sends delete collection {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Collection.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "deleteobject", "objtype": "collection", "me": u, "id": s })
end

Then('she sends purge count collections') do
   $lastResult = Send({ "type": "purgecount", "objtype": "collection" })
end

Then('she sends purge collections') do
   $lastResult = Send({ "type": "purge", "objtype": "collection" })
end

When('she sends set collection name of {string} to {string} as {string}') do |name, newname, user|
   u = User.where(name: user).first._id.to_s
   s = Collection.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setobject", "objtype": "collection", "me": u, "id": s, "name": newname })
end

When("there are {int} generated objs in collection {string} from {string} with policy {string}") do |count, s, u, p|

   collection = Collection.where(name: s).first._id.to_s
   user = User.where(name: u).first._id.to_s
   policy = Policy.where(name: p).first._id.to_s
   date = Date.today.at_beginning_of_month
   
   for i in 1..count do
      date = date + 5.minutes
      obj = FactoryBot.build(:obj)     
      obj.user = user
      obj.text = "Obj " + i.to_s
      obj.policy = policy
      obj.coll = collection
      obj.modifyDate = date
      obj.save
   end
   
end

When('she sends collections to downstream {int}') do |n|
   $lastResult = SendTo({ "type": "objects", "objtype": "collection" }, getDownstreamPort(n))
end

When('she sends set collection name of {string} to {string}') do |name, newname|
   s = Collection.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setobject", "objtype": "collection", "id": s, "name": newname })
end

When('she sends collection {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "object", "objtype": "collection", "collection": id }, getUpstreamPort())
end

Then('she sends collections to upstream') do
   $lastResult = SendTo({ "type": "objects", "objtype": "collection" }, getUpstreamPort())
end

When('she sends collection {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "object", "objtype": "collection", "collection": id }, getDownstreamPort(n))
end

When('she sends set collection name of {string} to {string} to upstream') do |id, newname|
   $lastResult = SendTo({ "type": "setobject", "objtype": "collection", "id": id, "name": newname }, getUpstreamPort())
end

When('she sends add collection {string} as {string} to downstream {int}') do |name, user, n|
   $lastResult = SendTo({ "type": "addobject", "objtype": "collection", "me": user, "name": name }, getDownstreamPort(n))
end

When('she sends add collection {string} as {string} to upstream') do |name, user|
   $lastResult = SendTo({ "type": "addobject", "objtype": "collection", "me": user, "name": name }, getUpstreamPort())
end

When('she sends add collection from upstream with {string}') do |id|
   $lastResult = Send({ "type": "addobject", "objtype": "collection", "upstream": true, "id": id })
end

When('she sends add collection from upstream with saved collection') do
   $lastResult = Send({ "type": "addobject", "objtype": "collection", "upstream": true, "id": $savedResult["result"] })
end
