Feature: Policy Users

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"
      
	Scenario: get the other users for a policy from the system
	   When she sends policy users as "p1"
      Then she receives policyusers
 