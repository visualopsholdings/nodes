require 'json'

# Send JSON to Nodes, regturninbg resulting JSON.

def SendToPorts(j, reqPort, subPort)
   s = JSON.generate(j)
   return JSON.parse(`build/Send --logLevel=info --subPort=#{subPort} --reqPort=#{reqPort} '#{s}'`)
end

def SendTo(j, subPort)
   reqPort = subPort.to_i + 1
   SendToPorts(j, reqPort, subPort)
end

def Send(j)
   SendToPorts(j, ENV['NODES_REP_PORT'], ENV['NODES_PUB_PORT'])
end

def calcPort(n)
   # 3012, 3023 etc
   return 3002 + (n * 10)
end


def getDownstreamPort(n)
   return calcPort(n)
end

def getUpstreamPort()
   return calcPort(1)
end