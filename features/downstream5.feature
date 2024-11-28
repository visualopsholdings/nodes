@nodes
Feature: Downstream 5 Test

   Background:

      And the server has id "15a7bfe2-6065-4c1f-926f-8bcf192c84c0"
      And the server has privateKey "7m:w>SDD88PEsyNoEtw+3%5Iryd@%^HA*VaY!6UF"
      And the server has pubKey "0:ZPruWnP/f>e&-HxPq{0WXL!Yizf#A%}:kj(Aa!"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "Oz-z+wnZOA{T^aVV{i7HHuCc0Bk}N/1G)*IApiP?"

      And there are users:
         | name   | admin  | fullname  | ui              | uibits    | id                         | modifyDate    |
         | admin  | true   | Admin     | conversations   | 293853    | 56348765b4d6976b478e1bd7   | Dec 4 2021    |
         | public | false  | Public    | conversations   | 7340036   | 5ed64555155a1f6043b954e7   | Apr 8 2021    |
         
      And there are groups:
         | name         | id                       | modifyDate   |
         | Admin        | 581c6d3bd81902a173c3ecd2 | Apr 8 2021   |
         | Everybody    | 601aa304d7a55b33de48707a | Apr 8 2021   |
         | Invited      | 601ce314d7a55b33de48bcd4 | Apr 8 2021   |
         | Public       | 5ef34b9a7264281391b4c72e | Apr 8 2021   |
         
      And there are policies:
         | name   | viewgroup             | editgroup                   | execgroup    |
         | p1     | Admin                 | Admin                       | Admin        |
      
      And groups have policies:
         | name         | policy |
         | Admin        | p1     |
         | Public       | p1     |
         
      And there are users in group "Admin":
         | name      |
         | admin     |
         
      And there are users in group "Public":
         | name      |
         | public     |
         
      And there are collections:
         | name             | policy | id                        | modifyDate |
         | Login Messages   | p1     | 61a7e24ab7726a10fd5254d4  | Jan 19 2018 |

      And the server has infos:
         | type            | text            |
         | upstreamMirror  | true    |

      And there are sites:
         | name      | headerTitle     | streamBgColor   |
         | main      | Downstream 5    | yellow          |

      And the node service is started

   @javascript @downstream5
	Scenario: Setup downstream 5 server
      Then eventually there are 1 collections in the DB
