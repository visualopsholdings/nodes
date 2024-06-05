Feature: Message

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c |
   
	Scenario: send a messsage to a user
	   When she sends message as "tracy"
      Then she receives ack
 