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

	Scenario: add a new group
	   When she sends add group "My Team" as "tracy"
	   And she sends groups as "tracy"
      Then she receives 3 groups

	Scenario: delete a group
	   When she sends add group "My Team" as "tracy"
	   And she sends groups as "tracy"
      And she receives 3 groups
	   And she sends delete group "My Team" as "tracy"
	   And she sends groups as "tracy"
      Then she receives 2 groups

	Scenario: set the name of a group
	   When she sends set group name of "Team 1" to "Other Team" as "tracy"
	   When she sends group "Other Team" as "tracy"
      Then she receives group "Other Team"
