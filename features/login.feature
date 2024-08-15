Feature: Login

   Background:
      When there is default security
      And the server has id "2d105350-8598-4377-b045-238194e23bc5"
  
	Scenario: Login to the system
	   When she sends login as "tracy"
      Then she receives user
 