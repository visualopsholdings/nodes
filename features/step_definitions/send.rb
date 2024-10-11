require 'json'

# Send JSON to Nodes, regturninbg resulting JSON.

def SendTo(j, reqPort, subPort)
   s = JSON.generate(j)
   return JSON.parse(`build/Send --logLevel=trace --subPort=#{subPort} --reqPort=#{reqPort} '#{s}'`)
end

def Send(j)
   SendTo(j, ENV['NODES_REP_PORT'], ENV['NODES_PUB_PORT'])
end
