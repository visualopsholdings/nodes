Feature: Policy Users

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c |
         | leanne    | false  | Leanne    | 6142d258ddf5aa5644057d35 |
   
#       And there are groups:
#          | name        | id                        |
#          | Team 1      | 61a58472ddf5aa463a08293d  |
 
      And there are policies:
         | name   | users        |
         | p1     | leanne tracy |
#          | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup |
#          | p1     |                    | Team 1    |                    | Team 1    |                    | Team 1    |

#       And there are users in group "Team 1":
#          | name      | roles           |
#          | tracy     | admin user      |
#          | leanne    | pleb user       |
         
#       And the security indexes are generated

	Scenario: get the other users for a policy from the system
	   When she sends policy users as "p1"
      Then she receives policyusers
 