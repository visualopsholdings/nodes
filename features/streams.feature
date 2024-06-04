Feature: Streams

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c |
   
	Scenario: get the streams from the system
	   When she sends streams as "u1"
      Then she receives streams
 