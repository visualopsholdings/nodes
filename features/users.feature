Feature: Users

   Background:
      When there is default security

	Scenario: get the users
	   When she sends users
      Then she receives 5 users

	Scenario: get a particular user
	   When she sends user "tracy"
      Then she receives user "Tracy"

	Scenario: delete a user
	   And she sends delete user "Bob"
	   And she sends users
      Then she receives 4 users
