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

When('she sends streams to downstream2') do
   $lastResult = SendTo({ "type": "streams" }, 3023, 3022)
end

When('she sends ideas for {string} to downstream2') do |id|
   $lastResult = SendTo({ "type": "ideas", "stream": id }, 3023, 3022)
end

When('she sends streams to downstream3') do
   $lastResult = SendTo({ "type": "streams" }, 3033, 3032)
end

When('she sends ideas for {string} to downstream3') do |id|
   $lastResult = SendTo({ "type": "ideas", "stream": id }, 3033, 3032)
end

When('she sends streams to downstream4') do
   $lastResult = SendTo({ "type": "streams" }, 3043, 3042)
end

When('she sends ideas for {string} to downstream4') do |id|
   $lastResult = SendTo({ "type": "ideas", "stream": id }, 3043, 3042)
end
