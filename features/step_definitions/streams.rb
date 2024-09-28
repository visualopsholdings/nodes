require 'json'

lastResult = nil

When('she sends streams as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   j = JSON.generate({ "type": "streams" , "me": u })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives {int} streams') do |count|
   expect(lastResult["type"]).to eq("streams")
   expect(lastResult["streams"].length).to eq(count)
end

When('she sends stream {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: name).first._id.to_s
   j = JSON.generate({ "type": "stream" , "me": u, "stream": s })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives stream {string}') do |name|
   expect(lastResult["type"]).to eq("stream")
   expect(lastResult["stream"]["name"]).to eq(name)
end
