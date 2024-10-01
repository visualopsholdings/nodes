require 'json'

When('she sends streams as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "streams" , "me": u })
end

Then('she receives {int} streams') do |count|
   expect($lastResult["type"]).to eq("streams")
   expect($lastResult["streams"].length).to eq(count)
end

When('she sends stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "stream" , "me": u, "stream": s })
end

Then('she receives stream {string}') do |name|
   expect($lastResult["type"]).to eq("stream")
   expect($lastResult["stream"]["name"]).to eq(name)
end

When('she sends add stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "addstream" , "me": u, "name": name })
end

When('she sends delete stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "deletestream" , "me": u, "id": s })
end

When('she sends set stream name of {string} to {string} as {string}') do |name, newname, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setstream" , "me": u, "id": s, "name": newname })
end
