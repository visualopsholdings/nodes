require 'json'

When('she sends streams as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "streams", "me": u })
end

When('she sends streams') do
   $lastResult = Send({ "type": "streams" })
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

When('she sends stream with id {string}') do |id|
   $lastResult = Send({ "type": "stream", "stream": id })
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

Then('she sends purge count streams') do
   $lastResult = Send({ "type": "purgecountstreams" })
end

Then('she sends purge streams') do
   $lastResult = Send({ "type": "purgestreams" })
end

When('she sends set stream name of {string} to {string} as {string}') do |name, newname, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setstream" , "me": u, "id": s, "name": newname })
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

When('she sends ideas for {string}') do |name|
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "ideas", "stream": s })
end

Then('she receives {int} ideas') do |count|
   expect($lastResult["type"]).to eq("ideas")
   expect($lastResult["ideas"].length).to eq(count)
end

When('she sends streams to downstream {int}') do |n|
   $lastResult = SendTo({ "type": "streams"}, getDownstreamPort(n))
end

When('she sends ideas for {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "ideas", "stream": id }, getDownstreamPort(n))
end

When('she sends set stream name of {string} to {string}') do |name, newname|
   s = Stream.where(name: name).first._id.to_s
   $lastResult = Send({ "type": "setstream", "id": s, "name": newname })
end

When('she sends stream {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "stream", "stream": id }, getUpstreamPort())
end

Then('she sends streams to upstream') do
   $lastResult = SendTo({ "type": "streams"}, getUpstreamPort())
end

When('she sends stream {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "stream", "stream": id }, getDownstreamPort(n))
end

When('she sends set stream name of {string} to {string} to upstream') do |id, newname|
   $lastResult = SendTo({ "type": "setstream", "id": id, "name": newname }, getUpstreamPort())
end

When('she sends add stream {string} as {string} to downstream {int}') do |name, user, n|
   $lastResult = SendTo({ "type": "addstream", "me": user, "name": name }, getDownstreamPort(n))
end

When('she sends add stream {string} as {string} to upstream') do |name, user|
   $lastResult = SendTo({ "type": "addstream", "me": user, "name": name }, getUpstreamPort())
end

When('she sends add stream from upstream with {string}') do |id|
   $lastResult = Send({ "type": "addstream", "upstream": true, "id": id })
end

When('she sends add stream from upstream with saved stream') do
   $lastResult = Send({ "type": "addstream", "upstream": true, "id": $savedResult["result"] })
end

When('she sends ideas for {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "ideas", "stream": id }, getUpstreamPort())
end
When('she sends ideas for saved stream to upstream') do
   $lastResult = SendTo({ "type": "ideas", "stream": $savedResult["result"] }, getUpstreamPort())
end
