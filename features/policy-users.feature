Feature: Policy Users

	Scenario: get the other users for a policy from the system
	   When she sends policy users as "p1"
      Then she receives policyusers
 