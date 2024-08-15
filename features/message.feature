Feature: Message

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |
         | My Conversation 2  | p1     |

	Scenario: send a messsage to a user
	   When she sends message "xxxxx" as "tracy" to "My Conversation 1"
      Then she receives ack
 