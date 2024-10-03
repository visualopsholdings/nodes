require 'json'

# Send JSON to Nodes, regturninbg resulting JSON.

def Send(j)
   s = JSON.generate(j)
   reqPort = ENV['NODES_REP_PORT']
   subPort = ENV['NODES_PUB_PORT']
   return JSON.parse(`build/Send --subPort=#{subPort} --reqPort=#{reqPort} '#{s}'`)
end
