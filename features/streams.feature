Feature: Streams

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"

      And there are streams:
         | name               | policy |
         | Conversation 1  | p1     |
         | Conversation 2  | p1     |

	Scenario: get the streams from the system
	   When she sends streams as "tracy"
      Then she receives streams
