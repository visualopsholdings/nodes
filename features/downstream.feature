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

#       And there are sequences:
#          | name               | id                          | upstream  | modifyDate   |
#          | Waiting discovery  | 6134e711a6041562149f4ef2    | true      | 1 Jan 1970   |
#          | Waiting discovery  | 59ca44ef404b132e9b3ce06b    | true      | 1 Jan 1970   |
#          | Waiting discovery  | 5e6980d0958ea618c4d34fe2    | true      | 1 Jan 1970   |
#  
#       And there are media:
#          | name               | id                          | upstream  | modifyDate   |
#          | Waiting discovery  | 643d0523ddf5aa60877582f6    | true      | 1 Jan 1970   |
         
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

#    @javascript @local
# 	Scenario: Add node to upstream server
#       When "tracy" log into other app "admin"
#  	   And she clicks "Nodes"
#       And eventually table has 2 rows
#  	   And she clicks "ADD"
#       And a modal dialog appears
#       And she enters "2d105350-8598-4377-b045-238194e23bc5" in "serverId"
#       And she enters "/9AxT<](&2X.HceOX#VoJ.zONJ#=*lgYu8hKgP*4" in "pubKey"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually table has 3 rows
#           
#    @javascript @local
# 	Scenario: Add upstream to downstream4 server
#       When "admin" log into other4 app "admin"
#  	   And she clicks "SET UPSTREAM"
#       And a modal dialog appears
#       And she enters "local" in "upstream"
#       And she enters "r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6(" in "upstreamPubKey"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       When "tracy" log into app "convo"
#       Then eventually there are 5 streams in the DB
          
   @javascript @downstream
	Scenario: Setup downstream server
      Then eventually there are 3 streams in the DB
      
#    @javascript
#    Scenario: Downstream 1 has synced
#       When "tracy" log into app "simpleadmin"
# 	   And she clicks "Users"
#       And eventually table has 2 rows
# 	   And she clicks "Teams"
#       And eventually table has 2 rows
# 	   And she clicks "Sequences"
#       And eventually table has 2 rows
# 	   And she clicks "Media"
#       And eventually table has 2 rows
#       And she clicks button named "edit" in row with text "SevenFortySeven"
#       And eventually table has 1 rows
# 
#    @javascript
#    Scenario: Downstreams have synced
#       When "tracy" log into app "conversations"
# 	   And eventually there are 2 streams
#  	   And eventually there are 30 ideas in the DB
# # 	   And eventually there are 10005 ideas in the DB
#  	   And she clicks "Stream 1"
#  	   Then eventually the page contains "1 – 20 of 20"
# # 	   Then eventually the page contains "1 – 30 of 10000"
# 
#       #downstream2
#       When "tracy" log into other2 app "conversations"
# 	   And eventually there are 3 streams
#  	   And she clicks "Shared Stream"
#  	   Then eventually the page contains "1 – 10 of 10"
# 
#       #downstream3
#       When "tracy" log into other3 app "conversations"
# 	   And eventually there are 2 streams
#  	   And she clicks "Shared Stream"
#  	   Then eventually the page contains "1 – 10 of 10"
# 
#       #downstream4
#       When "tracy" log into other4 app "conversations"
# 	   And eventually there are 4 streams
#  	   And she clicks "Stream 1"
# # 	   Then before the end of time the page contains "1 – 30 of 1000"
#  	   Then before the end of time the page contains "1 – 20 of 20"
#  	   And she clicks "Stream 2"
#  	   Then before the end of time the page contains "1 – 1 of 1"
# #  	   And she clicks "Shared Stream"
# #  	   Then before the end of time the page contains "1 – 5 of 5"

   @javascript
   Scenario: An existing user can be pulled from upstream
	   When she sends add user from upstream with "6142d258ddf5aa5644057d35"
      And she receives ack
      And eventually the user fullname "Leanne" with salt and hash appears in the DB
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Users"
#       And eventually table has 2 rows
#  	   And she clicks "ADD FROM UPSTREAM"
#       And a modal dialog appears
#       And she enters "Leanne" in "email"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And a modal dialog appears
#       And she checks the first list option
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "Discovery complete"
#       And she refreshes the page
#       Then eventually table has 3 rows
#       And eventually the user fullname "Leanne" with salt and hash appears in the DB
      
#    @javascript
#    Scenario: An existing user can be pushed downstream
#    
#       # downstream2      
#       When "tracy" log into other2 app "admin"
#   	   And she clicks "Users"
#       And eventually table has 1 rows
#       
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Nodes"
#       And she clicks button named "edit" in row with text "14ee64a1-d68f-4f6f-862a-d1ea23210010"
#  	   And she clicks "PUSH USER"
#       And a modal dialog appears
#       And she enters "Leanne" in "query"
#       And she clicks "CHOOSE"
#       And eventually the modal dialog disappears
#       And eventually the page contains "User pushed"
#       
#       # downstream2      
#       When "tracy" log into other2 app "admin"
#   	   And she clicks "Users"
#       And eventually table has 2 rows
#       
#       # downstream3
#       When "tracy" log into other3 app "admin"
#   	   And she clicks "Users"
#       And eventually table has 2 rows
#       
#  	@javascript
#    Scenario: Change to a user is reflected in the downstream servers
#    
#       # downstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Users"
#       And eventually table has 2 rows
#       And she clicks button named "edit" in row with text "Tracy"
#       And eventually the page contains "tracy@fe.com"
#       
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Users"
#       And eventually table has 4 rows
#       And she clicks button named "edit" in row with text "Tracy"
#       And she clicks "CHANGE EMAIL"
#       And a modal dialog appears
#       And she enters "x@x.com" in "email"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "x@x.com"
#       
#       # downstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Users"
#       And eventually table has 2 rows
#       And she clicks button named "edit" in row with text "Tracy"
#       And eventually the page contains "x@x.com"
#        
#       #downstream2
#       When "tracy" log into other2 app "admin"
#  	   And she clicks "Users"
#       And eventually table has 1 rows
#       And she clicks button named "edit" in row with text "Tracy"
#       And eventually the page contains "x@x.com"
# 
#       #downstream3
#       When "tracy" log into other3 app "admin"
#  	   And she clicks "Users"
#       And eventually table has 2 rows
#       And she clicks button named "edit" in row with text "Tracy"
#       And eventually the page contains "x@x.com"
# 
#       #downstream4
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Users"
#       And eventually table has 4 rows
#       And she clicks button named "edit" in row with text "Tracy"
#       And eventually the page contains "x@x.com"
# 
#    @javascript
#    Scenario: An existing team can be pulled from upstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#  	   And she clicks "ADD FROM UPSTREAM"
#       And a modal dialog appears
#       And she enters "Team 2" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And a modal dialog appears
#       And she checks the first list option
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "Discovery complete"
#       And she refreshes the page
#       Then eventually table has 3 rows
#       And eventually the group "Team 2" with 1 member appears in the DB
#       
#  	@javascript
#    Scenario: Change to a team is reflected in the downstream servers
#    
#       # downstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#       And eventually the page contains "Team 1"
#       
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#       And she clicks button named "edit" in row with text "Team 1"
#       And she enters "Team x" in "name"
# 	   And she clicks "SAVE"
#       And eventually the page contains "Team x"
#       
#       # downstream
#       Then eventually there is a group "Team x" in the DB
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#       And eventually the page contains "Team x"
# 
#       #downstream2
#       When "tracy" log into other2 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 1 rows
#       And eventually the page contains "Team x"
#        
#       #downstream3
#       When "tracy" log into other3 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#       And eventually the page contains "Team x"
#        
#       #downstream4
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 6 rows
#       And eventually the page contains "Team x"
#        
#    @javascript
#    Scenario: An existing sequence can be pulled from upstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Sequences"
#       And eventually table has 4 rows
#  	   And she clicks "ADD FROM UPSTREAM"
#       And a modal dialog appears
#       And she enters "Event" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And a modal dialog appears
#       And she checks the first list option
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "Discovery complete"
#       And she refreshes the page
#       Then eventually table has 5 rows
#       And eventually the sequence "Event" appears in the DB
#    
#    @javascript
#    Scenario: An existing media can be pulled from upstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Media"
#       And eventually table has 1 rows
#  	   And she clicks "ADD FROM UPSTREAM"
#       And a modal dialog appears
#       And she enters "Mum" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And a modal dialog appears
#       And she checks the first list option
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "Discovery complete"
#       And she refreshes the page
#       Then eventually table has 2 rows
#       And eventually the media "MumAndI" appears in the DB
#    
#    @javascript
#    Scenario: A deleted idea is reflected upstream and downstream servers
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 5 items
# 	   And she clicks menu on first action in conversation item with title "Message 5"
# 	   And she clicks "MOVE"
# 	   And she clicks "DELETE"
#       And a modal dialog appears
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually there are 4 items
#    
# 	   # back in upstream convo
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 4 items
# 	   
# 	   # back in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 9 items
# 	   
# 	   # back in downstream3 convo
#       When "tracy" log into other3 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 9 items
# 	   
# 	   # back in downstream4 convo
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 4 items
# 	   
#    @javascript
#    Scenario: An idea created owned and deleted is reflected upstream and downstream servers
#    
#       # in downstream
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 5 items
#       And she enters "My Idea" in "newText"
# 	   And she clicks "Idea"
# 	   And eventually there are 6 items
# 	   And she clicks "OWN"
# 	   And eventually the page contains "COMPLETE"
# 	   
#       # back in upstream convo
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 6 items
# 	   And eventually the page contains "COMPLETE"
# 
# 	   # back in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 11 items
# 	   And eventually the page contains "COMPLETE"
# 	   
#       # in downstream
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 6 items
# 	   And she clicks menu on first action in conversation item with title "My Idea"
# 	   And she clicks "MOVE"
# 	   And she clicks "DELETE"
#       And a modal dialog appears
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually there are 5 items
# 
#       # back in upstream convo
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 5 items
# 
# 	   # back in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 10 items
# 	   
# 	   # back in downstream3 convo
#       When "tracy" log into other3 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 10 items
# 	   
# 	   # back in downstream4 convo
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 5 items
# 	   
#    @javascript
#    Scenario: A renamed idea is reflected to downstream servers
#    
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 5 items
# 	   And she clicks menu on first action in conversation item with title "Message 5"
# 	   And she clicks "RENAME"
#       And a modal dialog appears
#       And she enters "Message 5 renamed" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       Then eventually there is a task "Message 5 renamed" in the DB
#    
# 	   # back in upstream convo
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
# 	   # back in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
# 	   # back in downstream3 convo
#       When "tracy" log into other3 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
# 	   # back in downstream4 convo
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
#    @javascript
#    Scenario: A renamed idea is reflected to upstream servers
#    
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually there are 5 items
# 	   And she clicks menu on first action in conversation item with title "Message 5"
# 	   And she clicks "RENAME"
#       And a modal dialog appears
#       And she enters "Message 5 renamed" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       Then eventually there is a task "Message 5 renamed" in the DB
#    
# 	   # back in upstream convo
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
# 	   # back in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
# 	   # back in downstream3 convo
#       When "tracy" log into other3 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
# 	   # back in downstream4 convo
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "Message 5 renamed"
# 	   
#  	@javascript
#    Scenario: Change to a sequence is reflected in the downstream servers
#    
#       # downstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Sequences"
#       And eventually table has 4 rows
#       And eventually the page contains "Count"
#       
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Count"
#       And she clicks "EDIT EMOJI"
#       And a modal dialog appears
#       And she enters "🧮" in "emoji"
#       And she clicks "Ok"
#       And eventually the page contains "🧮"
#       
#       # downstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Sequences"
#       And eventually table has 4 rows
#       And eventually the page contains "🧮"
#        
#       # downstream2
#       When "tracy" log into other2 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 2 rows
#       And eventually the page contains "🧮"
#        
#       # downstream3
#       When "tracy" log into other3 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 2 rows
#       And eventually the page contains "🧮"
#        
#       # downstream4
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And eventually the page contains "🧮"
#        
#  	@javascript
#    Scenario: Change to a stream is reflected in the downstream servers
#    
#       # downstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Streams"
#       And eventually table has 3 rows
#       And eventually the page contains "Shared Stream"
#       
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 4 rows
#       And she clicks button named "edit" in row with text "Shared Stream"
#       And she enters "Stream x" in "name"
# 	   And she clicks "SAVE"
#       And eventually the page contains "Stream x"
#       
#       # downstream
#       Then eventually there is a stream "Stream x" in the DB
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Streams"
#       And eventually table has 3 rows
#       And eventually the page contains "Stream x"
#        
#       # downstream2
#       When "tracy" log into other2 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 3 rows
#       And eventually the page contains "Stream x"
#        
#       # downstream3
#       When "tracy" log into other3 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 2 rows
#       And eventually the page contains "Stream x"
#        
#       # downstream4
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 5 rows
#       And eventually the page contains "Stream x"
#        
#    @javascript
#    Scenario: Using admin an existing stream can be pulled from upstream
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Streams"
#       And eventually table has 3 rows
#  	   And she clicks "ADD FROM UPSTREAM"
#       And a modal dialog appears
#       And she enters "Stream 2" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And a modal dialog appears
#       And she checks the first list option
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "Discovery complete"
#       And she refreshes the page
#       Then eventually table has 4 rows
#    
#       # in the downstream convo
#       When "tracy" log into app "conversations"
# 	   And eventually there are 3 streams
# 	   And she clicks "Stream 2"
# 	   And eventually there are 1 items
#       And she enters "My Idea" in "newText"
#       And she clicks new idea
# 	   Then eventually there are 2 items
# 	   
# 	   # back in upstream convo
#       When "tracy" log into other app "conversations"
# 	   And eventually there are 4 streams
# 	   And she clicks "Stream 2"
# 	   Then eventually there are 2 items
# 	   
#    @javascript
#    Scenario: An existing stream can be pulled from upstream
#    
#       # in the downstream convo
#       When "tracy" log into app "conversations"
# 	   And eventually there are 2 streams
#       And she clicks button named "search"
#       And she enters "Stream 2" in "query"
#       And she clicks button named "download"
#       And before the end of time the page contains "Download complete"
#       And she clicks button named "back"
#       And she clicks "Stream 2"
# 	   And eventually there are 1 items
#       And she enters "My Idea" in "newText"
#       And she clicks new idea
# 	   Then eventually there are 2 items
# 	   
# 	   # back in upstream convo
#       When "tracy" log into other app "conversations"
# 	   And eventually there are 4 streams
# 	   And she clicks "Stream 2"
# 	   Then eventually there are 2 items
# 	   
#  	@javascript
#    Scenario: A new stream with ideas can be pulled from upstream
#    
#       # in upstream convo
#       When "tracy" log into other app "conversations"
# 	   And eventually there are 4 streams
# 	   And she clicks button named "add-stream"
#       And a modal dialog appears
#       And she enters "New Stream" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "New Stream"
#  	   And she clicks "New Stream"
#       And she enters "Hi there" in "newText"
#       And she clicks "add-message"
#       And she enters "My Idea" in "newText"
#       And she clicks new idea
#  	   Then eventually there are 2 items
# 
#       # in downstream (admin)
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "Streams"
#       And eventually table has 3 rows
#  	   And she clicks "ADD FROM UPSTREAM"
#       And a modal dialog appears
#       And she enters "New Stream" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And a modal dialog appears
#       And she checks the first list option
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually the page contains "Discovery complete"
#       And she refreshes the page
#       Then eventually table has 4 rows
#  
#       # in the downstream convo
#       When "tracy" log into app "conversations"
# 	   And eventually there are 3 streams
# 	   And she clicks "New Stream"
# 	   And eventually there are 2 items
#       And she enters "I know" in "newText"
#       And she clicks "add-message"
# 	   Then eventually there are 3 items
# 	   
# 	   # back in upstream convo
#       When "tracy" log into other app "conversations"
# 	   And eventually there are 2 streams
# 	   And she clicks "New Stream"
# 	   Then eventually there are 3 items
# 
#  	@javascript
#    Scenario: Idea change is reflected from upstream to downstreams
#    
#       # in upstream convo
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
#       And she enters "My Idea" in "newText"
#       And she clicks new idea
# 	   Then eventually there are 6 items
# 
#       # in the downstream convo
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "OWN"	   
#       
#       # in upstream convo
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "OWN"	   
#       And she clicks "OWN"
#       And eventually the page contains "COMPLETE"	   
# 	   
#       # in the downstream convo
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
#       
#       # in the downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
#       
#       # in the downstream3 convo
#       When "tracy" log into other3 app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
#       
#       # in the downstream4 convo
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
#       
#  	@javascript
#    Scenario: Idea change is reflected from downstream2 to downstream
#    
#       # in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
#       And she enters "My Idea" in "newText"
#       And she clicks new idea
# 	   Then eventually there are 11 items
# #	   Then eventually there are 6 items
# 
#       # in the downstream convo
#       When "tracy" log into app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "OWN"	   
#       And she clicks "OWN"
#       And eventually the page contains "COMPLETE"	   
#       
#       # in downstream2 convo
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
# 
#       # in downstream3 convo
#       When "tracy" log into other3 app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
# 
#       # in downstream4 convo
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
#       And eventually the page contains "COMPLETE"	   
# 
#   	@javascript
#    Scenario: A new stream created on upstream appears on downstream mirrors
# 
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 5 rows
#  	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 4 rows
# 	   When she clicks "ADD"
#       And a modal dialog appears
#       And she enters "New Stream" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually table has 5 rows
#   	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 6 rows
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 6 rows
#  	   
#  	@javascript
#    Scenario: A stream created in a mirror downstream4 is reflected in the upstream
#    
#       # in downstream4 convo
#       When "admin" log into other4 app "conversations"
# 	   When she clicks button named "add-stream"
#       And a modal dialog appears
#       And she enters "New Stream" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#  	   Then eventually the page contains "New Stream"
#  	   
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 5 rows
#  	   Then eventually the page contains "New Stream"
#  
#  	@javascript
#    Scenario: A stream created in a mirror downstream6 is reflected in the upstream
#    
#       # in downstream6 convo
#       When "tracy" log into other6 app "conversations"
# 	   When she clicks button named "add-stream"
#       And a modal dialog appears
#       And she enters "New Stream" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#  	   Then eventually the page contains "New Stream"
#  	   
#       # upstream
#       When "tracy" log into other app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 5 rows
#  	   Then eventually the page contains "New Stream"
#  
#       # downstream4
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 6 rows
#  	   Then eventually the page contains "New Stream"
#  
#   	@javascript
#    Scenario: An idea can be branched from the downstream4 mirror
# 
#       # in downstream4 convo
#       When "admin" log into other4 app "conversations"
#       When she clicks "Stream 2"
#  	   And she clicks menu on first action in conversation item with title "Count 1"
#  	   And she clicks menu item "COMMENT"
#       And a modal dialog appears
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#  	   
#       # upstream
#       When "tracy" log into other app "conversations"
#       When she clicks "Stream 2"
#  	   Then eventually the page contains "No comments."
#  	   
#   	@javascript
#    Scenario: An idea can be branched from the downstream6 mirror
# 
#       # in downstream6 convo
#       When "admin" log into other6 app "conversations"
#       When she clicks "Stream 2"
#  	   And she clicks menu on first action in conversation item with title "Count 1"
#  	   And she clicks menu item "COMMENT"
#       And a modal dialog appears
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#  	   
#       # upstream
#       When "tracy" log into other app "conversations"
#       When she clicks "Stream 2"
#  	   Then eventually the page contains "No comments."
#  	   
#       # downstream4
#       When "admin" log into other4 app "conversations"
#       When she clicks "Stream 2"
#  	   Then eventually the page contains "No comments."
#  	   
#   	@javascript
#    Scenario: A new sequence created on upstream appears on downstream mirrors
# 
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#  	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#  	   When she clicks "NEW"
#       And eventually table has 7 rows
#   	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 7 rows
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 7 rows
#  	   
#   	@javascript
#    Scenario: A new sequence created on downstream4 appears on upstream
# 
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#  	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#  	   When she clicks "NEW"
#       And eventually table has 7 rows
#   	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 7 rows
#  	   
#   	@javascript
#    Scenario: A new sequence created on downstream6 appears on upstream
# 
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#  	   When she clicks "NEW"
#       And eventually table has 7 rows
#   	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 7 rows
#  	   
#       # in dowwnstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 7 rows
#  	   
#   	@javascript
#    Scenario: A new draft sequence created on upstream appears on downstream mirrors
# 
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "no draft"
#  	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "no draft"
#  	   And she clicks "CREATE DRAFT"
#  	   And eventually the page contains "a draft"
# 	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "a draft"
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "a draft"
#  	   
#   	@javascript
#    Scenario: A new draft sequence created on downstream4 appears on upstream
# 
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "no draft"
#  	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   And eventually the page contains "no draft"
#  	   And she clicks "CREATE DRAFT"
#  	   And eventually the page contains "a draft"
#   	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "a draft"
#  	   
#   	@javascript
#    Scenario: A new draft sequence created on downstream6 appears on upstream
# 
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "no draft"
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "no draft"
#  	   And she clicks "CREATE DRAFT"
#  	   And eventually the page contains "a draft"
#   	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "a draft"
#  	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Sequences"
#       And eventually table has 6 rows
#       And she clicks button named "edit" in row with text "Event"
#  	   Then eventually the page contains "a draft"
#  	   
#   	@javascript
#    Scenario: A new team created on upstream appears on downstream mirrors
# 
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 6 rows
#  	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
# 	   When she clicks "ADD"
#       And a modal dialog appears
#       And she enters "New Team" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually table has 3 rows
#       And she clicks button named "edit" in row with text "New Team"
# 	   And she clicks "ADD MEMBER"
#       And eventually a modal dialog appears
#       And she enters "Tracy" in "query"
#       And she clicks button named "add" in row with text "Tracy"
#       And eventually the modal dialog disappears
#       And eventually table has 1 rows
#   	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 7 rows
#       And she clicks button named "edit" in row with text "New Team"
#       And eventually table has 1 rows
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 7 rows
#       And she clicks button named "edit" in row with text "New Team"
#       And eventually table has 1 rows
#  	   
#   	@javascript
#    Scenario: A new team created on downstream4 appears on upstream
# 
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#  	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 6 rows
# 	   When she clicks "ADD"
#       And a modal dialog appears
#       And she enters "New Team" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually table has 7 rows
#   	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 3 rows
#  	   
#   	@javascript
#    Scenario: A new team created on downstream6 appears on upstream
# 
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 2 rows
#  	   
#       # in downstream6 admin
#       When "admin" log into other6 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 6 rows
# 	   When she clicks "ADD"
#       And a modal dialog appears
#       And she enters "New Team" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
#       And eventually table has 7 rows
#   	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 3 rows
#  	   
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#  	   And she clicks "Teams"
#       And eventually table has 7 rows
#  	   
#   	@javascript
#    Scenario: A certificate can be requested from the upstream
# 
#       # in downstream4 admin
#       When "admin" log into other4 app "admin"
#       When she clicks "Sites"
#       And she clicks button named "edit" in row with text "main"
#       And she clicks "REQUEST FROM UPSTREAM"
#       And a modal dialog appears
#       And she clicks "Ok"
#   	   Then eventually the page contains "Certificate requested"
# 
#   	@javascript
#    Scenario: A new stream can be created on downstream5
# 
#       # in downstream5
#       When "tracy" log into other5 app "conversations"
# 	   When she clicks button named "add-stream"
#       And a modal dialog appears
#       And she enters "New Conversation" in "name"
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   Then eventually the page contains "New Conversation"
#   	   And she waits 3 seconds
#  	   
#       # downstream2
#       When "tracy" log into other2 app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 4 rows
#       And eventually the page contains "New Conversation"
#  	   
#       # in upstream admin
#       When "tracy" log into other app "admin"
#  	   And she clicks "Streams"
#       And eventually table has 4 rows
# 
#    @javascript
#    Scenario: An idea can be moved from one stream to another on a mirror
#    
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Stream 2"
# 	   And eventually the page contains "1 – 1 of 1"
#  	   And she clicks menu on first action in conversation item with title "Count 1"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Stream" in "query"
#       And she clicks stream "Shared Stream" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "0 of 0"
#   	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
# 	   
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Stream 2"
# 	   And eventually the page contains "0 of 0"
#   	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
#       
#    @javascript
#    Scenario: An idea can be moved from a shared stream to a non shared stream on downstream
#    
#       # downstream2
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#  	   And she clicks menu on first action in conversation item with title "Message 3"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Stream" in "query"
#       And she clicks stream "Stream 4" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "1 – 4 of 4"
#   	   And she clicks "Stream 4"
# 	   And eventually the page contains "1 – 1 of 1"
# 	   
#       # downstream4
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream5
#       When "tracy" log into other5 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream6
#       When "tracy" log into other6 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream2
#       When "tracy" log into other2 app "conversations"
#       And she clicks "Stream 4"
# 	   And eventually the page contains "1 – 1 of 1"
#  	   And she clicks menu on first action in conversation item with title "Message 3"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Shared" in "query"
#       And she clicks stream "Shared Stream" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "0 of 0"
# 
#       # downstream4
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#       # downstream5
#       When "tracy" log into other5 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#       # downstream6
#       When "tracy" log into other6 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#    @javascript
#    Scenario: An idea can be moved from a shared stream to a non shared stream on mirror
#    
#       # downstream4
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Stream 2"
# 	   And eventually the page contains "1 – 1 of 1"
#  	   And she clicks menu on first action in conversation item with title "Count 1"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Stream" in "query"
#       And she clicks stream "Shared Stream" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "0 of 0"
# 
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
# 
#       # downstream2
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
#       
#       # downstream5
#       When "tracy" log into other5 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
#       
#       # downstream6
#       When "tracy" log into other6 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
#       
#    @javascript
#    Scenario: An idea can be moved between shared streams
#    
#       # downstream2
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#  	   And she clicks menu on first action in conversation item with title "Message 3"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Stream" in "query"
#       And she clicks stream "Shared 2 Stream" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "1 – 4 of 4"
#   	   And she clicks "Shared 2 Stream"
# 	   And eventually the page contains "1 – 1 of 1"
# 	   
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream4
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream5
#       When "tracy" log into other5 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream6
#       When "tracy" log into other6 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 4 of 4"
#       
#       # downstream2
#       When "tracy" log into other2 app "conversations"
#       And she clicks "Shared 2 Stream"
# 	   And eventually the page contains "1 – 1 of 1"
#  	   And she clicks menu on first action in conversation item with title "Message 3"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Shared" in "query"
#       And she clicks stream "Shared Stream" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "0 of 0"
# 
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#       # downstream4
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#       # downstream5
#       When "tracy" log into other5 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#       # downstream6
#       When "tracy" log into other6 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 5 of 5"
#       
#    @javascript
#    Scenario: An idea can be moved from a non shared stream to a shared stream on upstream
#    
#       # upstream
#       When "tracy" log into other app "conversations"
#  	   And she clicks "Stream 2"
# 	   And eventually the page contains "1 – 1 of 1"
#  	   And she clicks menu on first action in conversation item with title "Count 1"
# 	   And she clicks "MOVE"
# 	   And she clicks "MOVE TO STREAM"
#       And a modal dialog appears
#       And she enters "Stream" in "query"
#       And she clicks stream "Shared Stream" in modal dialog
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "0 of 0"
# 
#       # downstream2
#       When "tracy" log into other2 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
#       
#       # downstream4
#       When "tracy" log into other4 app "conversations"
#  	   And she clicks "Shared Stream"
# 	   And eventually the page contains "1 – 6 of 6"
# 
#    @javascript
#    Scenario: A build can be downloaded
#       When "tracy" log into app "simpleadmin"
#  	   And she clicks "REQUEST BUILD"
#       And a modal dialog appears
#       And she clicks "Ok"
#       And eventually the modal dialog disappears
# 	   And eventually the page contains "Finished receiving build"
# 
# 	@javascript
# 	Scenario: A new media is reflected in a mirror
# 	
# 	   # upstream
#       When "tracy" log into other app "admin"
# 	   And she clicks "Media"
#       And eventually table has 2 rows
# 	   And she clicks "ADD IMAGE"
#       And a modal dialog appears
#       And she enters "An image" in "name"
#       And she uploads file "media/image.png"
#       And she clicks "Upload"
#       And eventually the modal dialog disappears
#       Then eventually table has 3 rows
# 
#       # downstream4
#       When "tracy" log into other4 app "admin"
# 	   And she clicks "Media"
#       Then eventually table has 3 rows

