Feature: Collections

   Background:
      When there is default security

      And there are collections:
         | name         | policy |
         | Collection 1 | p1     |
         | Collection 2 | p1     |

	Scenario: get the collections
	   When she sends collections as "tracy"
      Then she receives 2 collections

	Scenario: get a particular collection
	   When she sends collection "Collection 2" as "tracy"
      Then she receives collection "Collection 2"

	Scenario: not allowed to get a particular collection
	   When she sends collection "Collection 2" as "bob"
      Then she receives security error

	Scenario: add a new collection
	   When she sends add collection "My Object" as "tracy"
	   And she sends collections as "tracy"
      Then she receives 3 collections

	Scenario: delete a collection
	   When she sends add collection "My Object" as "tracy"
	   And she sends collections as "tracy"
      And she receives 3 collections
	   And she sends delete collection "My Object" as "tracy"
	   And she sends collections as "tracy"
      Then she receives 2 collections
	   And she sends purge count collections
      And she receives 1 count
	   Then she sends purge collections

	Scenario: not allowed to delete a collection
	   When she sends add collection "My Object" as "tracy"
	   And she sends collections as "tracy"
      And she receives 3 collections
	   And she sends delete collection "My Object" as "bob"
      Then she receives security error

	Scenario: set the name of a collection
	   When she sends set collection name of "Collection 1" to "Other Collection" as "tracy"
	   When she sends collection "Other Collection" as "tracy"
      Then she receives collection "Other Collection"

	Scenario: not allowed to set the name of a collection
	   When she sends set collection name of "Collection 1" to "Other Collection" as "bob"
      Then she receives security error
