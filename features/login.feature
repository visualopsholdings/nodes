Feature: Login

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c |
   
	Scenario: Login to the system
	   When she sends login as "tracy"
      Then she receives user
 