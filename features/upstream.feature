@nodes
Feature: Upstream Test

   Background:
      When there are users:
         | name      | admin  | fullname  | id                       | modifyDate   |
         | admin     | true   | Admin     | 56348765b4d6976b478e1bd7 | Dec 4 2021   |
         | public    | false  | Public    | 5ed64555155a1f6043b954e7 | Apr 8 2021   |
         | tracy     | true   | Tracy     | 6121bdfaec9e5a059715739c | 2 Jan 2022   |
         | leanne    | false  | Leanne    | 6142d258ddf5aa5644057d35 | 2 Jan 2022   |
          
      And there are groups:
         | name        | id                        | modifyDate   |
         | Team 1      | 613d8641ec9e5a6c4785d6d2  | 2 Jan 2022   |
         | Team 2      | 61fb4fdcddf5aa9052809bd7  | 2 Jan 2022   |
         
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
         | p1     |                    | Team 1    |                    | Team 1    |                    | Team 1    | 6386dbabddf5aaf74ca1e1f6 |
         | p2     |                    | Team 2    |                    | Team 2    |                    | Team 2    |                          |
         | p3     | tracy              |           | tracy              |           | tracy              |           | 64365effddf5aa03b2c092a3 |
         | p4     | leanne             |           | leanne             |           | leanne             |           |                          |
         
     And the security indexes are generated
         
      And groups have policies:
         | name         | policy |
         | Team 1       | p1     |
         | Team 2       | p2     |
 
      And there are collections:
         | name                | policy | id                       | modifyDate   |
         | Collection 1        | p1     | 613ee472ec9e5aafc85e1301 | 4 Jan 2022   |
         | Collection 2        | p1     | 61444c6addf5aaa6a02e05b7 | 5 Jan 2022   |
         | Shared Collection   | p1     | 61a0b4de98499a20f0768351 | 2 Jan 2022   |
         | Shared 2 Collection | p1     | 63c0c08e337cce14a54e0517 | 2 Jan 2022   |

      And there are objs:
         | name            | modifyDate   | policy | collection   | by     | id                       |
         | Count 1         | Sep 2 2021   | p1     | Collection 2 | tracy  | 67455997ca25979b57a61319 |

      And there are 20 generated objs in collection "Collection 1" from "tracy" with policy "p1"
      And there are 10 generated objs in collection "Shared Collection" from "tracy" with policy "p1"

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
   