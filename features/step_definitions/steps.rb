require 'json'

$lastResult = nil

Then('she receives error {string}') do |msg|
   expect($lastResult["type"]).to eq("err")
   expect($lastResult["msg"]).to match(msg)
end

Then('she receives security error') do
   expect($lastResult["type"]).to eq("err")
   expect($lastResult["level"]).to eq("security")
end

When('she sends login as {string}') do |username|
   $lastResult = Send({ "type": "login" , "session": "1", "password": username })
end

Then('she receives user') do
   expect($lastResult["type"]).to eq("user")
end

Then('she receives ack') do
   expect($lastResult["type"]).to eq("ack")
end

When('she sends policy users as {string}') do |id|
   policy = Policy.where(name: id).first._id.to_s
   $lastResult = Send({ "type": "policyusers" , "policy": policy })
end

Then('she receives policyusers') do
   expect($lastResult["type"]).to eq("policyusers")
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
   $lastResult = Send({ "type": "policy", "objtype": objtype,  "id": id })
end

Then('she receives policy {string}') do |string|
   expect($lastResult["type"]).to eq("policy")
end

When('the node service is started') do
   ServiceManager.start
end

When("she waits {int} seconds") do |n|
  sleep(n.to_i)
end

When("she saves the result") do
   $savedResult = $lastResult
end

