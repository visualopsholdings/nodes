require 'json'

lastResult = nil

When('she sends users') do
   j = JSON.generate({ "type": "users" })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives {int} users') do |count|
   expect(lastResult["type"]).to eq("users")
   expect(lastResult["users"].length).to eq(count)
end

When('she sends user {string}') do |name|
   u = User.where(name: name).first._id.to_s
   j = JSON.generate({ "type": "user" , "user": u })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives user {string}') do |name|
   expect(lastResult["type"]).to eq("user")
   expect(lastResult["user"]["fullname"]).to eq(name)
end
