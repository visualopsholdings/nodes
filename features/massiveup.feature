@nodes
Feature: Massive Upstream Test

   Background:
      When there are users:
         | name      | admin  | fullname  | id     | modifyDate   |
         | tracy     | true   | Tracy     | $tracy | 2 Jan 2022   |
          
      And there are groups:
         | name        | id                        | modifyDate   |
         | Team 1      | $Team 1  | 2 Jan 2022   |
         
      And there are users in group "Team 1":
         | name      | roles          |
         | tracy     | admin user     |
         
      And there are policies:
         | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup | id                 | 
         | p1     |                    | Team 1    |                    | Team 1    |                    | Team 1    | $All Team 1 Policy |
         
      And the security indexes are generated
         
      And groups have policies:
         | name         | policy |
         | Team 1       | p1     |
 
      And there are collections:
         | name                  | policy | id                         | modifyDate   |
         | Shared Collection     | p1     | $Shared Collection       | 4 Jan 2022   |

      And there are 10000 generated objs in collection "Shared Collection" from "tracy" with policy "p1"

      And the server has id "4ecaf2b6-42f2-49fe-b0ef-123758a3a4db"
      And the server has privateKey "t5R[&?W.8-mjIc]cs#<T(5AW7sCJYHo^<2k-]!2!"
      And the server has pubKey "31r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6("
      
      And the server has nodes:
         | id                                   | pubKey                                   |
         | 2d105350-8598-4377-b045-238194e23bc5 | /9AxT<](&2X.HceOX#VoJ.zONJ#=*lgYu8hKgP*4 |
         | 14ee64a1-d68f-4f6f-862a-d1ea23210010 | Oz-z+wnZOA{T^aVV{i7HHuCc0Bk}N/1G)*IApiP? |
         | f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6 | P[hw!7:n1Q&SU#epb>:<FbvG*V$N>3Hn.[&c++E1 |
         
      And there are sites:
         | name         | initialApp      | initialPubApp      | headerTitle  | streamBgColor   |
         | localhost    | conversations   | conversations      | Upstream     | lightblue       |
 
      And the node service is started

   @javascript @massiveup
	Scenario: Setup massive upstream server
      Then eventually there are 1 collections in the DB	
   