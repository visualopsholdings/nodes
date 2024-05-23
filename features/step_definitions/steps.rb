require 'json'

lastResult = nil

When('she sends login as {string}') do |username|
   lastResult = JSON.parse(`build/Send --cmd=login --arg="#{username}"`)
end

Then('she receives user') do
   expect(lastResult["type"]).to eq("user")
end

When('she sends message as {string}') do |user|
   lastResult = JSON.parse(`build/Send --cmd=message --arg="#{user}"`)
end

Then('she receives ack') do
   expect(lastResult["type"]).to eq("ack")
end

When('she sends policy users as {string}') do |policy|
   lastResult = JSON.parse(`build/Send --cmd=policyusers --arg="#{policy}"`)
end

Then('she receives policyusers') do
   expect(lastResult["type"]).to eq("policyusers")
end

When('she sends streams as {string}') do |user|
   lastResult = JSON.parse(`build/Send --cmd=streams --arg="#{user}"`)
end

Then('she receives streams') do
   expect(lastResult["type"]).to eq("streams")
end
