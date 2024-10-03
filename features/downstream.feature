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
