@nodes
Feature: Downstream 2 Test

   Background:
      And there are users:
         | name      | fullname           | id                          | upstream  | modifyDate   | ui              | uibits    | admin  |
         | tracy     | Tracy              | 6121bdfaec9e5a059715739c    | true      | 1 Jan 2022   | conversations   | 293853    | true   |
         
      And there are groups:
         | name               | id                          | upstream  | modifyDate   |
         | Team 1             | 613d8641ec9e5a6c4785d6d2    | true      | 1 Jan 2022   |
         
      And there are users in group "Team 1":
         | name      |
         | tracy     |
         
      And there are policies:
         | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup | id                       | 
         | p1     |                    | Team 1    |                    | Team 1    |                    | Team 1    | 6386dbabddf5aaf74ca1e1f6 |
         | p3     | tracy              |           | tracy              |           | tracy              |           | 64365effddf5aa03b2c092a3 |
         
      And groups have policies:
         | name         | policy |
         | Team 1       | p1     |
 
      And there are streams:
         | name               | id                          | upstream  | modifyDate   | policy |
         | Shared Stream      | 61a0b4de98499a20f0768351    | true      | 1 Jan 2022   | p1     |
         | Shared 2 Stream    | 63c0c08e337cce14a54e0517    | true      | 2 Jan 2022   | p1     |
         | Stream 4           | 637aa99202e727169a58282f    |           | 2 Jan 2022   | p1     |

#       And there are sequences:
#          | name               | id                       | data                     | upstream  | modifyDate   | policy |
#          | Count              | 6134e711a6041562149f4ef2 | testdata/seq/count.json  | true      | 2 Jan 2022   | p1     |
# 
      And the server has id "14ee64a1-d68f-4f6f-862a-d1ea23210010"
      And the server has privateKey "ul(R&i4F6yjAI8+iZD&8*/2p4Uk7VESxs9NnwdLB"
      And the server has pubKey "Oz-z+wnZOA{T^aVV{i7HHuCc0Bk}N/1G)*IApiP?"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "31r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6("
      And the server has hasInitialSync
      And the server has upstreamLastSeen "3 Jan 2022"

#       And there are media:
#          | name               | policy    |
#          | Mum                | p1        |
#          
#       And there are media:
#          | name               | id                          | upstream  | modifyDate   |
#          | Waiting discovery  | 643d0523ddf5aa60877582f7    | true      | 1 Jan 1970   |
#          
#       And there are formats for media "Mum":
#          | name         | uuid                        | type  |
#          | thumbnail    | llllllll-mmmm-nnnn-oooooooo | image |
# 
      And there are sites:
         | name      | headerTitle     | streamBgColor   | headerImage           |
         | main      | Downstream 2    | lightred        | Mum                   |
 
      And the server has nodes:
         | id                                   | pubKey                                   |
         | e4448086-8a22-4db0-9dcf-c95a56418d77 | Xe$)#}CyZ%<!U[d8RrMV@ahzQ-QO4$3.3#n}$*Iu |
         | 15a7bfe2-6065-4c1f-926f-8bcf192c84c0 | 0:ZPruWnP/f>e&-HxPq{0WXL!Yizf#A%}:kj(Aa! |
         
      And the node service is started

   @javascript @downstream2
	Scenario: Setup downstream 2 server
      Then eventually there are 3 streams in the DB
      
