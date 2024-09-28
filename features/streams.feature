Feature: Streams

   Background:
      When there is default security

      And there are streams:
         | name               | policy |
         | Conversation 1  | p1     |
         | Conversation 2  | p1     |

	Scenario: get the streams
	   When she sends streams as "tracy"
      Then she receives 2 streams

	Scenario: get a particular stream
	   When she sends stream "Conversation 2" as "tracy"
      Then she receives stream "Conversation 2"
