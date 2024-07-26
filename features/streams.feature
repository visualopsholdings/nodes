Feature: Streams

   Background:
      When there is default security

      And there are streams:
         | name               | policy |
         | Conversation 1  | p1     |
         | Conversation 2  | p1     |

	Scenario: get the streams from the system
	   When she sends streams as "tracy"
      Then she receives streams
