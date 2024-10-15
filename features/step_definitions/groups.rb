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

When('she sends set group name of {string} to {string} to upstream') do |id, newname|
   $lastResult = SendTo({ "type": "setgroup", "id": id, "name": newname }, 3013, 3012)
end

When('she sends group {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "group", "group": id }, 3013, 3012)
end

When('she sends group {string} to downstream2') do |id|
   $lastResult = SendTo({ "type": "group", "group": id }, 3023, 3022)
end

When('she sends group {string} to downstream3') do |id|
   $lastResult = SendTo({ "type": "group", "group": id }, 3033, 3032)
end

When('she sends group {string} to downstream4') do |id|
   $lastResult = SendTo({ "type": "group", "group": id }, 3043, 3042)
end
