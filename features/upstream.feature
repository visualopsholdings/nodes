@nodes
Feature: Upstream Test

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       | modifyDate   |
         | admin     | true   | Admin     | $admin | Dec 4 2021   |
         | public    | false  | Public    | $public | Apr 8 2021   |
         | tracy     | true   | Tracy     | $tracy | 2 Jan 2022   |
         | leanne    | false  | Leanne    | $leanne | 2 Jan 2022   |
          
      And there are groups:
         | name        | id                        | modifyDate   |
         | Team 1      | $Team 1  | 2 Jan 2022   |
         | Team 2      | $Team 2  | 2 Jan 2022   |
         
      And there are users in group "Team 1":
         | name      | roles          |
         | tracy     | admin user     |
         | leanne    | pleb user      |
         | admin     |                |
         
      And there are users in group "Team 2":
         | name      | roles          |
         | leanne    | user     |

      And there are policies:
         | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup | id                       | 
         | p1     |                    | Team 1    |                    | Team 1    |                    | Team 1    | $All Team 1 Policy |
         | p2     |                    | Team 2    |                    | Team 2    |                    | Team 2    |                          |
         | p3     | tracy              |           | tracy              |           | tracy              |           | $All Tracy Policy |
         | p4     | leanne             |           | leanne             |           | leanne             |           |                          |
         
      And the security indexes are generated
         
      And groups have policies:
         | name         | policy |
         | Team 1       | p1     |
         | Team 2       | p2     |
 
      And there are collections:
         | name                | policy | id                         | modifyDate   |
         | Collection 1        | p1     | $Collection 1              | 4 Jan 2022   |
         | Collection 2        | p1     | $Collection 2              | 5 Jan 2022   |
         | Shared Collection   | p1     | $Shared Collection         | 2 Jan 2022   |
         | Shared 2 Collection | p1     | $Shared 2 Collection       | 2 Jan 2022   |

      And there are objs:
         | name            | modifyDate   | policy | collection         | by     | id              |
         | Count 1         | Dec 24 2022  | p1     | Collection 2       | tracy  | $Count 1        |
         | Shared Obj X    | Dec 25 2022  | p1     | Shared Collection  | tracy  | $Shared Obj X   |

      And there are 500 generated objs in collection "Collection 1" with prefix "Obj " from "tracy" with policy "p1" at "2022-7-25"
      And there are 100 generated objs in collection "Shared Collection" with prefix "Shared Obj " from "tracy" with policy "p1" at "2022-8-25"

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

   @javascript @upstream
	Scenario: Setup upstream server
      Then eventually there are 4 collections in the DB	
   