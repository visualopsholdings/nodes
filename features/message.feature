Feature: Message

   Background:
      When there is default security

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |
         | My Conversation 2  | p1     |

	Scenario: send a messsage to a user
	   When she sends message "xxxxx" as "tracy" to "My Conversation 1"
      Then she receives ack
 