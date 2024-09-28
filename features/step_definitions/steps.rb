require 'json'

lastResult = nil

When('she sends login as {string}') do |username|
   j = JSON.generate({ "type": "login" , "session": "1",  "password": username })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives user') do
   expect(lastResult["type"]).to eq("user")
end

When('she sends message {string} as {string} to {string}') do |text, user, stream|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: stream).first._id.to_s
   j = JSON.generate({ "type": "message" , "me": u,  "stream": s, "text": text, "corr": "1" })
   lastResult = JSON.parse(`build/Send --logLevel=debug '#{j}'`)
end

Then('she receives ack') do
   expect(lastResult["type"]).to eq("ack")
end

When('she sends policy users as {string}') do |id|
   policy = Policy.where(name: id).first._id.to_s
   j = JSON.generate({ "type": "policyusers" , "policy": policy })
   lastResult = JSON.parse(`build/Send '#{j}'`)
end

Then('she receives policyusers') do
   expect(lastResult["type"]).to eq("policyusers")
end

When('she sends policy for {string} named {string}') do |objtype, name |
   id = ""
   if objtype == "stream"
      id = Stream.where(name: name).first._id.to_s
   elsif objtype == "group"
      id = Group.where(name: name).first._id.to_s
   else
      puts "bad objtype"
   end 
   j = JSON.generate({ "type": "policy", "objtype": objtype,  "id": id })
   lastResult = JSON.parse(`build/Send --logLevel=debug '#{j}'`)
end

Then('she receives policy {string}') do |string|
   expect(lastResult["type"]).to eq("policy")
end
