
When('she sends message {string} as {string} to {string}') do |text, user, stream|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: stream).first._id.to_s
   $lastResult = Send({ "type": "message" , "me": u,  "stream": s, "text": text, "corr": "1" })
end

When('she sends message {string} as {string} to saved stream to upstream') do |text, user|
   $lastResult = SendTo({ "type": "message" , "me": user,  "stream": $savedResult["result"], "text": text, "corr": "1" }, getUpstreamPort())
end

When('she sends delete idea {string} as {string} in {string}') do |text, user, stream|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: stream).first._id.to_s
   i = Idea.where(text: text, stream: s).first._id.to_s
   $lastResult = Send({ "type": "deleteobject", "objtype": "idea", "me": u, "id": i })
end

When('she sends ideas for {string} as {string}') do |stream, user|
   s = Stream.where(name: stream).first._id.to_s
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "objects", "objtype": "idea", "me": u, "stream": s })
end

Then('she receives {int} ideas') do |count|
   expect($lastResult["type"]).to eq("ideas")
   expect($lastResult["ideas"].length).to eq(count)
end

When('she sends ideas for {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "objects", "objtype": "idea", "stream": id }, getDownstreamPort(n))
end

When('she sends ideas for {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "objects", "objtype": "idea", "stream": id }, getUpstreamPort())
end

When('she sends ideas for saved stream to upstream') do
   $lastResult = SendTo({ "type": "objects", "objtype": "idea", "stream": $savedResult["result"] }, getUpstreamPort())
end
