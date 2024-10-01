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

	Scenario: add a new stream
	   When she sends add stream "My Stream" as "tracy"
	   And she sends streams as "tracy"
      Then she receives 3 streams

	Scenario: delete a stream
	   When she sends add stream "My Stream" as "tracy"
	   And she sends streams as "tracy"
      And she receives 3 streams
	   And she sends delete stream "My Stream" as "tracy"
	   And she sends streams as "tracy"
      Then she receives 2 streams

	Scenario: set the name of a stream
	   When she sends set stream name of "Conversation 1" to "Other Conversation" as "tracy"
	   When she sends stream "Other Conversation" as "tracy"
      Then she receives stream "Other Conversation"
