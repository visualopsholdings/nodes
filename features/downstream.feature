@nodes
Feature: Downstream Test

   Background:
      And there are users:
         | fullname           | id                          | upstream  | password  | modifyDate   | admin |
         | Waiting discovery  | 6121bdfaec9e5a059715739c    | true      | no        | 1 Jan 1970   | true  |
         
       When there are users:
         | name      | admin  | fullname  | email           | ui              | uibits    |
         | alice     | false  | Alice     | alice@fe.com    | conversations   | 293853    |
         
      And there are groups:
         | name               | id                          | upstream  | modifyDate   |
         | Waiting discovery  | 613d8641ec9e5a6c4785d6d2    | true      | 1 Jan 1970   |
         
      And there are groups:
         | name        |
         | Team 3      |
         
      And there are users in group "Team 3":
         | name      | roles          |
         | alice     | user     |

      And there are policies:
         | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup |
         | p3     |                    | Team 3    |                    | Team 3    |                    | Team 3    |
         
      And there are streams:
         | name               | id                          | upstream  | modifyDate   |
         | Waiting discovery  | 613ee472ec9e5aafc85e1301    | true      | 1 Jan 1970   |
         | Waiting discovery  | 61a0b4de98499a20f0768351    | true      | 1 Jan 1970   |

      And there are streams:
         | name      | policy |
         | Stream 4  | p3     |

      And the server has id "2d105350-8598-4377-b045-238194e23bc5"
      And the server has privateKey "sM[0CH.io/U2[jvLMbB@UEU4Z]lTX@EDG2#DD7UK"
      And the server has pubKey "/9AxT<](&2X.HceOX#VoJ.zONJ#=*lgYu8hKgP*4"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "31r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6("
      
      And there are sites:
         | name      | headerTitle     | streamBgColor   |
         | main      | Downstream 1    | lightgreen      |
 
      And the node service is started
#      And the DB is new
      And she waits 1 seconds

   @javascript @downstream
	Scenario: Setup downstream server
      Then eventually there are 3 streams in the DB
      
   @javascript
   Scenario: An existing user can be pulled from upstream
	   When she sends add user from upstream with "6142d258ddf5aa5644057d35"
      And she receives ack
      And eventually the user fullname "Leanne" with salt and hash appears in the DB

   @javascript
   Scenario: An existing group can be pulled from upstream
	   When she sends add group from upstream with "61fb4fdcddf5aa9052809bd7"
      And she receives ack
      And eventually the group "Team 2" with 1 member appears in the DB

   @javascript
   Scenario: Downstreams have synced
	   When she sends streams
      Then she receives 3 streams
	   When she sends ideas for "Stream 1"
      Then she receives 20 ideas

      When she sends streams to downstream 2
      Then she receives 3 streams
	   When she sends ideas for "61a0b4de98499a20f0768351" to downstream 2
      Then she receives 15 ideas
      
      When she sends streams to downstream 3
      Then she receives 2 streams
	   When she sends ideas for "61a0b4de98499a20f0768351" to downstream 3
      Then she receives 15 ideas

      When she sends streams to downstream 4
      Then she receives 5 streams
	   When she sends ideas for "613ee472ec9e5aafc85e1301" to downstream 4
      Then she receives 20 ideas
	   When she sends ideas for "61444c6addf5aaa6a02e05b7" to downstream 4
      Then she receives 1 ideas
	   When she sends ideas for "61a0b4de98499a20f0768351" to downstream 4
      Then she receives 10 ideas

 	@javascript
   Scenario: Change to a user is reflected in the downstream servers
   
      When she sends set user fullname of "tracy" to "Joan"
      
      And she sends user "6121bdfaec9e5a059715739c" to upstream
      And she receives user "Joan"
      
      And she sends set user fullname of "6121bdfaec9e5a059715739c" to "Sue" to upstream
      And she waits 1 seconds
      
      And she sends user with id "6121bdfaec9e5a059715739c"
      And she receives user "Sue"

      And she sends user "6121bdfaec9e5a059715739c" to downstream 2
      And she receives user "Sue"
      
      And she sends user "6121bdfaec9e5a059715739c" to downstream 3
      And she receives user "Sue"
      
      And she sends user "6121bdfaec9e5a059715739c" to downstream 4
      And she receives user "Sue"

 	@javascript
   Scenario: Change to a group is reflected in the downstream servers
   
      When she sends set group name of "Team 1" to "New Group"
      
      And she sends group "613d8641ec9e5a6c4785d6d2" to upstream
      And she receives group "New Group"

      And she sends set group name of "613d8641ec9e5a6c4785d6d2" to "Another Group" to upstream
      And she waits 1 seconds

      And she sends group with id "613d8641ec9e5a6c4785d6d2"
      And she receives group "Another Group"

      And she sends group "613d8641ec9e5a6c4785d6d2" to downstream 2
      And she receives group "Another Group"
      
      And she sends group "613d8641ec9e5a6c4785d6d2" to downstream 3
      And she receives group "Another Group"
      
      And she sends group "613d8641ec9e5a6c4785d6d2" to downstream 4
      And she receives group "Another Group"
 
  	@javascript
   Scenario: Change to a stream is reflected in the downstream servers
   
      When she sends set stream name of "Shared Stream" to "Stream x"

      And she sends stream "61a0b4de98499a20f0768351" to upstream
      And she receives stream "Stream x"

      And she sends set stream name of "61a0b4de98499a20f0768351" to "Stream y" to upstream
      And she waits 1 seconds

      And she sends stream with id "61a0b4de98499a20f0768351"
      And she receives stream "Stream y"

      And she sends stream "61a0b4de98499a20f0768351" to downstream 2
      And she receives stream "Stream y"
      
      And she sends stream "61a0b4de98499a20f0768351" to downstream 3
      And she receives stream "Stream y"
      
      And she sends stream "61a0b4de98499a20f0768351" to downstream 4
      And she receives stream "Stream y"

  	@javascript
   Scenario: A new stream can be created on downstream 5

      When she sends add stream "New Conversation" as "56348765b4d6976b478e1bd7" to downstream 5
	   And she sends streams to downstream 5
      Then she receives 5 streams

      # 5 is a mirror of upstream2
	   And she sends streams to downstream 2
      Then she receives 4 streams
      
      # upstream shouldn't get it.
	   And she sends streams to upstream
      Then she receives 4 streams

  	@javascript
   Scenario: A new stream created on upstream appears on downstream mirrors

	   When she sends streams to downstream 4
      And she receives 5 streams
 	   
      And she sends add stream "New Stream" as "6121bdfaec9e5a059715739c" to upstream
	   And she sends streams to upstream
      And she receives 5 streams
  	   
	   And she sends streams to downstream 4
      And she receives 6 streams
 	   
	   And she sends streams to downstream 6
      Then she receives 6 streams
 	   
  	@javascript
   Scenario: A new group created on upstream appears on downstream mirrors

	   When she sends groups to downstream 4
      And she receives 6 groups
 	   
      And she sends add group "New Group" as "6121bdfaec9e5a059715739c" to upstream
	   And she sends groups to upstream
      And she receives 3 groups
  	   
	   And she sends groups to downstream 4
      And she receives 7 groups
 	   
	   And she sends groups to downstream 6
      Then she receives 7 groups

 	@javascript
   Scenario: A stream created in a mirror downstream 4 is reflected in the upstream
   
      When she sends add stream "New Stream" as "56348765b4d6976b478e1bd7" to downstream 4
	   And she sends streams to downstream 4
      And she receives 6 streams

	   And she sends streams to upstream
      Then she receives 5 streams

  	@javascript
   Scenario: A new group created on downstream 4 appears on upstream

      When she sends add group "New Group" as "56348765b4d6976b478e1bd7" to downstream 4
	   And she sends groups to downstream 4
      And she receives 7 groups

	   And she sends groups to upstream
      Then she receives 3 groups

 	@javascript
   Scenario: A stream created in a mirror downstream 6 is reflected in the upstream
   
      When she sends add stream "New Stream" as "6121bdfaec9e5a059715739c" to downstream 6
	   And she sends streams to downstream 6
      And she receives 6 streams

	   And she sends streams to upstream
      And she receives 5 streams

	   And she sends streams to downstream 4
      Then she receives 6 streams

  	@javascript
   Scenario: A new group created on downstream 6 appears on upstream

      When she sends add group "New Group" as "6121bdfaec9e5a059715739c" to downstream 6
	   And she sends groups to downstream 6
      And she receives 7 groups

	   And she sends groups to upstream
      And she receives 3 groups

	   And she sends groups to downstream 4
      Then she receives 7 groups

   @javascript
   Scenario: An existing stream can be pulled from upstream
   
      When she sends add stream from upstream with "61444c6addf5aaa6a02e05b7"
      And she receives ack
      And eventually the stream "Stream 2" has 1 ideas in the DB
      
	   And she sends streams
      And she receives 4 streams

      And she sends message "My Idea" as "tracy" to "Stream 2"
      And she sends ideas for "Stream 2"
      And she receives 2 ideas

	   And she sends ideas for "61444c6addf5aaa6a02e05b7" to upstream
      Then she receives 2 ideas

 	@javascript
   Scenario: A new stream with ideas can be pulled from upstream
   
      # add a new stream to the upstream with 2 new ideas.
      When she sends add stream "New Stream" as "6121bdfaec9e5a059715739c" to upstream
      And she saves the result
      And she sends message "My Idea 1" as "6121bdfaec9e5a059715739c" to saved stream to upstream
      And she sends message "My Idea 2" as "6121bdfaec9e5a059715739c" to saved stream to upstream
	   And she sends ideas for saved stream to upstream
      And she receives 2 ideas
      
      # make sure nothing came down.
	   When she sends streams
      And she receives 3 streams
      Then eventually there are 30 ideas in the DB
      
      # get the the stream from upstream.
      When she sends add stream from upstream with saved stream
      And she receives ack
	   And she sends streams
      And she receives 4 streams
      Then eventually there are 32 ideas in the DB
      
      # add a new message to this upstream stream.
      When she sends message "I know" as "tracy" to "New Stream"
      Then eventually the stream "New Stream" has 3 ideas in the DB
      
	   When she sends ideas for saved stream to upstream
      Then she receives 3 ideas
