lastResult = ""

When('she sends login as {string}') do |username|

   lastResult = `build/Send --cmd=login --arg="#{username}"`

end

Then('she receives user') do
   expect(lastResult).to eq("user")
end

When('she sends message as {string}') do |user|
   lastResult = `build/Send --cmd=message --arg="#{user}"`
end

Then('she receives ack') do
   expect(lastResult).to eq("ack")
end

When('she sends policy users as {string}') do |policy|
   lastResult = `build/Send --cmd=policyusers --arg="#{policy}"`
end

Then('she receives policyusers') do
   expect(lastResult).to eq("policyusers")
end

When('she sends streams as {string}') do |user|
   lastResult = `build/Send --cmd=streams --arg="#{user}"`
end

Then('she receives streams') do
   expect(lastResult).to eq("streams")
end
