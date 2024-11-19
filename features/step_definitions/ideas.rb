
When('she sends message {string} as {string} to {string}') do |text, user, stream|
   u = User.where(name: user).first._id.to_s
   s = Stream.where(name: stream).first._id.to_s
   $lastResult = Send({ "type": "message" , "me": u,  "stream": s, "text": text, "corr": "1" })
end

When('she sends message {string} as {string} to saved stream to upstream') do |text, user|
   $lastResult = SendTo({ "type": "message" , "me": user,  "stream": $savedResult["result"], "text": text, "corr": "1" }, getUpstreamPort())
end

When('she sends delete idea {string} as {string}') do |text, user|
   u = User.where(name: user).first._id.to_s
   i = Idea.where(text: text).first._id.to_s
   $lastResult = Send({ "type": "deleteidea" , "me": u, "id": i })
end
