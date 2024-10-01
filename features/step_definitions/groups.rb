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
   $lastResult = Send({ "type": "setgroup" , "me": u, "id": g, "name": newname })
end
