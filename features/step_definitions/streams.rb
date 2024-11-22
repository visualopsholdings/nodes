require 'json'

When('she sends streams as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "objects", "objtype": "stream", "me": u })
end

When('she sends streams') do
   $lastResult = Send({ "type": "objects", "objtype": "stream" })
end

Then('she receives {int} streams') do |count|
   expect($lastResult["type"]).to eq("streams")
   expect($lastResult["streams"].length).to eq(count)
end

When('she sends stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "object", "objtype": "stream", "me": u, "stream": s })
end

When('she sends stream with id {string}') do |id|
   $lastResult = Send({ "type": "object", "objtype": "stream", "stream": id })
end

Then('she receives stream {string}') do |name|
   expect($lastResult["type"]).to eq("stream")
   expect($lastResult["stream"]["name"]).to eq(name)
end

When('she sends add stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "addobject", "objtype": "stream", "me": u, "name": name })
end

When('she sends delete stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "deleteobject", "objtype": "stream", "me": u, "id": s })
end

Then('she sends purge count streams') do
   $lastResult = Send({ "type": "purgecount", "objtype": "stream" })
end

Then('she sends purge streams') do
   $lastResult = Send({ "type": "purge", "objtype": "stream" })
end

When('she sends set stream name of {string} to {string} as {string}') do |name, newname, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setobject", "objtype": "stream", "me": u, "id": s, "name": newname })
end

When("there are {int} generated messages in stream {string} from {string} with policy {string}") do |count, s, u, p|

   stream = Stream.where(name: s).first._id.to_s
   user = User.where(name: u).first._id.to_s
   policy = Policy.where(name: p).first._id.to_s
   date = Date.today.at_beginning_of_month
   
   for i in 1..count do
      date = date + 5.minutes
      message = FactoryBot.build(:idea)     
      message.user = user
#      message.date = date
      message.text = "Message " + i.to_s
      message.policy = policy
#      message.sortDate = date
      message.stream = stream
      message.modifyDate = date
      message.save
   end
   
end

When('she sends streams to downstream {int}') do |n|
   $lastResult = SendTo({ "type": "objects", "objtype": "stream" }, getDownstreamPort(n))
end

When('she sends set stream name of {string} to {string}') do |name, newname|
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setobject", "objtype": "stream", "id": s, "name": newname })
end

When('she sends stream {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "object", "objtype": "stream", "stream": id }, getUpstreamPort())
end

Then('she sends streams to upstream') do
   $lastResult = SendTo({ "type": "objects", "objtype": "stream" }, getUpstreamPort())
end

When('she sends stream {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "object", "objtype": "stream", "stream": id }, getDownstreamPort(n))
end

When('she sends set stream name of {string} to {string} to upstream') do |id, newname|
   $lastResult = SendTo({ "type": "setobject", "objtype": "stream", "id": id, "name": newname }, getUpstreamPort())
end

When('she sends add stream {string} as {string} to downstream {int}') do |name, user, n|
   $lastResult = SendTo({ "type": "addobject", "objtype": "stream", "me": user, "name": name }, getDownstreamPort(n))
end

When('she sends add stream {string} as {string} to upstream') do |name, user|
   $lastResult = SendTo({ "type": "addobject", "objtype": "stream", "me": user, "name": name }, getUpstreamPort())
end

When('she sends add stream from upstream with {string}') do |id|
   $lastResult = Send({ "type": "addobject", "objtype": "stream", "upstream": true, "id": id })
end

When('she sends add stream from upstream with saved stream') do
   $lastResult = Send({ "type": "addobject", "objtype": "stream", "upstream": true, "id": $savedResult["result"] })
end
