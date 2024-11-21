@nodes
Feature: Downstream 4 Test

   Background:

      And the server has id "f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6"
      And the server has privateKey "BDRys9WwOwY33X#7J3DN-IZQ}au2iNf/OcGN8fxu"
      And the server has pubKey "P[hw!7:n1Q&SU#epb>:<FbvG*V$N>3Hn.[&c++E1"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "31r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6("

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
         
      And there are streams:
         | name             | policy | id                        | modifyDate |
         | Login Messages   | p1     | 61a7e24ab7726a10fd5254d4  | Jan 19 2018 |

      And the server has upstreamMirror

      And there are sites:
         | name      | headerTitle     | streamBgColor   |
         | main      | Downstream 4    | blue            |

       And the server has nodes:
         | id                                   | pubKey                                   |
         | 05fd392d-1092-4255-8323-38c774e4f6a8 | Ar(px<?(^2j/>7s{]J+4R>klM=!+OHyWT#^pPbKE |
         
     And the node service is started

   @javascript @downstream4
	Scenario: Setup downstream 4 server
      Then eventually there are 1 streams in the DB