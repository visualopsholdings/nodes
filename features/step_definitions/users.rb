require 'json'

When('she sends users') do
   $lastResult = Send({ "type": "users" })
end

Then('she receives {int} users') do |count|
   expect($lastResult["type"]).to eq("users")
   expect($lastResult["users"].length).to eq(count)
end

When('she sends user {string}') do |name|
   u = User.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "user" , "user": u })
end

When('she sends user with id {string}') do |u|
   $lastResult = Send({ "type": "user" , "user": u })
end

Then('she receives user {string}') do |name|
   expect($lastResult["type"]).to eq("user")
   expect($lastResult["user"]["fullname"]).to eq(name)
end

Then('she sends delete user {string}') do |fullname|
   u = User.where(fullname: fullname).first._id.to_s
   $lastResult = Send({ "type": "deleteuser", "id": u })
end

When('she sends add user from upstream with {string}') do |id|
   $lastResult = Send({ "type": "adduser", "upstream": true, "id": id })
end

When('she sends set user name of {string} to {string}') do |name, newname|
   u = User.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setuser", "id": u, "name": newname })
end

When('she sends set user fullname of {string} to {string}') do |name, newname|
   u = User.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setuser", "id": u, "fullname": newname })
end

When('she sends user {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "user" , "user": id }, 3013, 3012)
end

When('she sends set user fullname of {string} to {string} to upstream') do |u, newname|
   $lastResult = SendTo({ "type": "setuser", "id": u, "fullname": newname }, 3013, 3012)
end

When('she sends user {string} to downstream2') do |id|
   $lastResult = SendTo({ "type": "user" , "user": id }, 3023, 3022)
end

When('she sends user {string} to downstream3') do |id|
   $lastResult = SendTo({ "type": "user" , "user": id }, 3033, 3032)
end

When('she sends user {string} to downstream4') do |id|
   $lastResult = SendTo({ "type": "user" , "user": id }, 3043, 3052)
end
