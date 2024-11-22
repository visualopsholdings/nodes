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
   # same defaults as inside local.sh
   subPort = ENV['NODES_PUB_PORT']
   if !subPort
      subPort = 3012
   end
   repPort = ENV['NODES_REP_PORT']
   if !repPort
      repPort = 3013
   end
   SendToPorts(j, repPort, subPort)
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