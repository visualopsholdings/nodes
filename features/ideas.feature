Feature: Ideas

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |
         | My Conversation 2  | p1     |

      And there are ideas:
         | name            | modifyDate   | policy | stream             | by     |
         | Message 1       | Sep 2 2021   | p1     | My Conversation 1  | tracy  |
         | Message 2       | Sep 2 2021   | p1     | My Conversation 1  | tracy  |
         | Message 3       | Sep 2 2021   | p1     | My Conversation 2  | tracy  |
         | Message 4       | Sep 2 2021   | p1     | My Conversation 2  | tracy  |
         | Message 5       | Sep 2 2021   | p1     | My Conversation 2  | tracy  |

	Scenario: get the ideas
      And she sends ideas for "My Conversation 2" as "tracy"
      And she receives 3 ideas

	Scenario: send a messsage to a user
	   When she sends message "xxxxx" as "tracy" to "My Conversation 1"
      Then she receives ack
 
	Scenario: delete an idea
 	   When she sends delete idea "Message 3" as "tracy" in "My Conversation 2"
      And she sends ideas for "My Conversation 2" as "tracy"
      Then she receives 2 ideas
