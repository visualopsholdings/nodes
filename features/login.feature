Feature: Login

   Background:
      When there is default security
   
	Scenario: Login to the system
	   When she sends login as "tracy"
      Then she receives user
 