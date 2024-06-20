require 'json'

lastResult = nil

When('she sends login as {string}') do |username|
   lastResult = JSON.parse(`build/Send --cmd=login --args="#{username}"`)
end

Then('she receives user') do
   expect(lastResult["type"]).to eq("user")
end

When('she sends message {string} as {string} to {string} policy {string}') do |text, user, stream, policy|
   u = User.where(name: user).first._id.to_s
   s = Policy.where(name: policy).first._id.to_s
   p = Stream.where(name: stream).first._id.to_s
   lastResult = JSON.parse(`build/Send --cmd=message --args="#{u},#{s},#{p},#{text}"`)
end

Then('she receives ack') do
   expect(lastResult["type"]).to eq("ack")
end

When('she sends policy users as {string}') do |id|
   policy = Policy.where(name: id).first._id.to_s
   lastResult = JSON.parse(`build/Send --cmd=policyusers --args="#{policy}"`)
end

Then('she receives policyusers') do
   expect(lastResult["type"]).to eq("policyusers")
end

When('she sends streams as {string}') do |user|
   u = User.where(name: user).first._id.to_s
   lastResult = JSON.parse(`build/Send --cmd=streams --args="#{u}"`)
end

Then('she receives streams') do
   expect(lastResult["type"]).to eq("streams")
end
