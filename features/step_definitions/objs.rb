
When('she sends obj {string} as {string} to {string}') do |text, user, coll|
   u = User.where(name: user).first._id.to_s
   s = Collection.where(name: coll).first._id.to_s
   $lastResult = Send({ "type": "addobject", "objtype": "obj", "me": u, "coll": s, "text": text, "corr": "1" })
end

When('she sends obj {string} as {string} to saved collection to upstream') do |text, user|
   $lastResult = SendTo({ "type": "addobject", "objtype": "obj", "me": user, "coll": $savedResult["result"], "text": text, "corr": "1" }, getUpstreamPort())
end

When('she sends delete obj {string} as {string} in {string}') do |text, user, coll|
   u = User.where(name: user).first._id.to_s
   s = Collection.where(name: coll).first._id.to_s
   i = Obj.where(text: text, coll: s).first._id.to_s
   $lastResult = Send({ "type": "deleteobject", "objtype": "obj", "me": u, "id": i })
end

When('she sends objs for {string} as {string}') do |coll, user|
   s = Collection.where(name: coll).first._id.to_s
   u = User.where(name: user).first._id.to_s
   $lastResult = Send({ "type": "objects", "objtype": "obj", "me": u, "coll": s })
end

Then('she receives {int} objs') do |count|
   expect($lastResult["type"]).to eq("objs")
   expect($lastResult["objs"].length).to eq(count)
#   puts $lastResult["objs"]
end

When('she sends objs for {string} to downstream {int}') do |id, n|
   $lastResult = SendTo({ "type": "objects", "objtype": "obj", "coll": id }, getDownstreamPort(n))
end

When('she sends objs for {string} as {string} to downstream {int}') do |id, uid, n|
   $lastResult = SendTo({ "type": "objects", "objtype": "obj", "coll": id, "me": uid }, getDownstreamPort(n))
end

When('she sends objs for {string} to upstream') do |id|
   $lastResult = SendTo({ "type": "objects", "objtype": "obj", "coll": id }, getUpstreamPort())
end

When('she sends objs for saved collection to upstream') do
   $lastResult = SendTo({ "type": "objects", "objtype": "obj", "coll": $savedResult["result"] }, getUpstreamPort())
end

When('she sends move obj {string} to {string} as {string} to downstream {int}') do |id, coll, uid, n|
   $lastResult = SendTo({ "type": "moveobject", "objtype": "obj", "id": id, "coll": coll, "me": uid }, getDownstreamPort(n))
end
