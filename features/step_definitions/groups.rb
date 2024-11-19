require 'json'

When('she sends groups as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "groups" , "me": u })
end

Then('she receives {int} groups') do |count|
   expect($lastResult["type"]).to eq("groups")
   expect($lastResult["groups"].length).to eq(count)
end

When('she sends group {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   g = Group.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "group" , "me": u, "group": g })
end

When('she sends group with id {string}') do |id|
   $lastResult = Send({ "type": "group", "group": id })
end

Then('she receives group {string}') do |name|
   expect($lastResult["type"]).to eq("group")
   expect($lastResult["group"]["name"]).to eq(name)
end

When('she sends members {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   g = Group.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "members" , "me": u, "group": g })
end

Then('she receives {int} members') do |count|
   expect($lastResult["type"]).to eq("members")
   expect($lastResult["group"]["members"].length).to eq(count)
end

When('she sends add group {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "addgroup" , "me": u, "name": name })
end

When('she sends delete group {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   g = Group.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "deletegroup" , "me": u, "id": g })
end

When('she sends set group name of {string} to {string} as {string}') do |name, newname, user|
   u = User.where(name: user).first._id.to_s
   g = Group.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setgroup", "me": u, "id": g, "name": newname })
end

When('she sends add group from upstream with {string}') do |id|
   $lastResult = Send({ "type": "addgroup", "upstream": true, "id": id })
end

When('she sends set group name of {string} to {string}') do |name, newname|
   g = Group.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setgroup", "id": g, "name": newname })
end

Then('she sends purge count groups') do
   $lastResult = Send({ "type": "purgecountgroups" })
end

Then('she sends purge groups') do
   $lastResult = Send({ "type": "purgegroups" })
end

When('she sends groups to downstream {int}') do |n|
   $lastResult = SendTo({ "type": "groups"}, getDownstreamPort(n))
end

When('she sends groups to upstream') do
   $lastResult = SendTo({ "type": "groups"}, getUpstreamPort())
end

When('she sends add group {string} as {string} to upstream') do |name, user|
   $lastResult = SendTo({ "type": "addgroup", "me": user, "name": name }, getUpstreamPort())
end

When('she sends set group name of {string} to {string} to upstream') do |id, newname|
   $lastResult = SendTo({ "type": "setgroup", "id": id, "name": newname }, getUpstreamPort())
end

When('she sends group {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "group", "group": id }, getUpstreamPort())
end

When('she sends group {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "group", "group": id }, getDownstreamPort(n))
end

When('she sends add group {string} as {string} to downstream {int}') do |name, user, n|
   $lastResult = SendTo({ "type": "addgroup", "me": user, "name": name }, getDownstreamPort(n))
end
