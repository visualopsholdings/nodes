@nodes
Feature: Downstream 3 Test

   Background:
      When there are users:
         | name      | admin  | fullname  | ui              | uibits    |
         | fred      | true   | Fred      | conversations   | 293853    |

      And there are users:
         | name   | fullname           | id        | upstream  | modifyDate   | ui              | uibits    | admin  |
         | tracy  | Tracy              | $tracy    | true      | 1 Jan 2022   | conversations   | 293853    | true   |

      And there are groups:
         | name               | id                          | upstream  | modifyDate   |
         | Team 1 Y           | $Team 1    | true      | 3 Jan 2022   |
         
      And there are users in group "Team 1 Y":
         | name      |
         | tracy     |
         
      And there are policies:
         | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup | id                 | 
         | p1     |                    | Team 1 Y  |                    | Team 1 Y  |                    | Team 1 Y  | $All Team 1 Policy |
         
      And there are groups:
         | name        |
         | Team X      |
         
      And groups have policies:
         | name         | policy |
         | Team 1 Y     | p1     |
 
      And there are collections:
         | name               | id                    | upstream  | modifyDate   | policy |
         | Shared Collection  | $Shared Collection    | true      | 2 Jan 2022   | p1     |
         | Collection 4       | $Collection 4         | true      | 4 Jan 2022   | p1     |

      And there are 5 generated objs in collection "Shared Collection" with prefix "DS3 Shared Obj " from "fred" with policy "p1" at "2023-07-25"

      And the server has id "e4448086-8a22-4db0-9dcf-c95a56418d77"
      And the server has privateKey "RHVqBD^IzF.?]i&*Y{&)0bWz@PZai>[]D8)62x-N"
      And the server has pubKey "Xe$)#}CyZ%<!U[d8RrMV@ahzQ-QO4$3.3#n}$*Iu"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "Oz-z+wnZOA{T^aVV{i7HHuCc0Bk}N/1G)*IApiP?"
      And the server has hasInitialSync
      And the server has upstreamLastSeen "3 Jan 2022"

      And there are sites:
         | name      | headerTitle     | streamBgColor   |
         | main      | Downstream 3    | orange          |

      And the node service is started

   @javascript @downstream3
	Scenario: Setup downstream 3 server
      Then eventually there are 2 collections in the DB
