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

Then('she receives user {string}') do |name|
   expect($lastResult["type"]).to eq("user")
   expect($lastResult["user"]["fullname"]).to eq(name)
end

Then('she sends delete user {string}') do |fullname|
   u = User.where(fullname: fullname).first._id.to_s
   $lastResult = Send({ "type": "deleteuser", "id": u })
end

When('she sends add user from upstream with {string}') do |id|
   $lastResult = Send({ "type": "newuser", "upstream": true, "id": id })
end
