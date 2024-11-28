Feature: Objs

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"

      And there are collections:
         | name            | policy |
         | My Collection 1 | p1     |
         | My Collection 2 | p1     |

      And there are objs:
         | name            | modifyDate   | policy | collection         | by     |
         | Obj 1        | Sep 2 2021   | p1     | My Collection 1  | tracy  |
         | Obj 2        | Sep 2 2021   | p1     | My Collection 1  | tracy  |
         | Obj 3        | Sep 2 2021   | p1     | My Collection 2  | tracy  |
         | Obj 4        | Sep 2 2021   | p1     | My Collection 2  | tracy  |
         | Obj 5        | Sep 2 2021   | p1     | My Collection 2  | tracy  |

	Scenario: get the objs
      And she sends objs for "My Collection 2" as "tracy"
      And she receives 3 objs

	Scenario: send an obj to a user
	   When she sends obj "xxxxx" as "tracy" to "My Collection 1"
      Then she receives ack
 
	Scenario: delete an obj
 	   When she sends delete obj "Obj 3" as "tracy" in "My Collection 2"
      And she sends objs for "My Collection 2" as "tracy"
      Then she receives 2 objs
