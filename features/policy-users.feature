Feature: Policy Users

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c |
         | leanne    | false  | Leanne    | 6142d258ddf5aa5644057d35 |
   
      And there are policies:
         | name   | users        |
         | p1     | leanne tracy |

	Scenario: get the other users for a policy from the system
	   When she sends policy users as "p1"
      Then she receives policyusers
 