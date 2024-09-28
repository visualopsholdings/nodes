Feature: Policies

   Background:
      When there is default security
 
	Scenario: get a particular groups policy
	   When she sends policy for "group" named "Team 1"
      Then she receives policy "Team 1"
