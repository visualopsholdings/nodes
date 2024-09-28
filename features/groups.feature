Feature: Groups

   Background:
      When there is default security
 
	Scenario: get the groups
	   When she sends groups as "tracy"
      Then she receives 2 groups

	Scenario: get a particular group
	   When she sends group "Team 1" as "tracy"
      Then she receives group "Team 1"

	Scenario: get a groups members
	   When she sends members "Team 1" as "tracy"
      Then she receives 3 members
