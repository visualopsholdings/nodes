Feature: Manual

   Background:
      When there is default security

      And there are streams:
         | name               | policy |
         | My Conversation 1  | p1     |
         | My Conversation 2  | p1     |

	Scenario: setup for manual testing
	   When she sends login as "tracy"
      Then she receives user
 