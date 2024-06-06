Feature: Streams

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c |
         | leanne    | false  | Leanne    | 6142d258ddf5aa5644057d35 |
   
      And there are policies:
         | name   | users        |
         | p1     | leanne tracy |

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |
         | My Conversation 2  | p1     |

	Scenario: get the streams from the system
	   When she sends streams as "tracy"
      Then she receives streams
 