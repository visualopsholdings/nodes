require 'json'

lastResult = nil

When('she sends groups as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   j = JSON.generate({ "type": "groups" , "me": u })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives {int} groups') do |count|
   expect(lastResult["type"]).to eq("groups")
   expect(lastResult["groups"].length).to eq(count)
end

When('she sends group {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   g = Group.where(name: name).first._id.to_s
   j = JSON.generate({ "type": "group" , "me": u, "group": g })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives group {string}') do |name|
   expect(lastResult["type"]).to eq("group")
   expect(lastResult["group"]["name"]).to eq(name)
end

When('she sends members {string} as {string}') do |name, user|
   u = User.where(name: user).first._id.to_s
   g = Group.where(name: name).first._id.to_s
   j = JSON.generate({ "type": "members" , "me": u, "group": g })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives {int} members') do |count|
   expect(lastResult["type"]).to eq("members")
   expect(lastResult["group"]["members"].length).to eq(count)
end
