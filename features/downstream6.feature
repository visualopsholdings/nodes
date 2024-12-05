@nodes
Feature: Downstream 6 Test

   Background:

      And the server has id "05fd392d-1092-4255-8323-38c774e4f6a8"
      And the server has privateKey "nCMAS5nOWyJ20lI/MV%*CFocjc8s?)aS()%K(TJb"
      And the server has pubKey "Ar(px<?(^2j/>7s{]J+4R>klM=!+OHyWT#^pPbKE"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "P[hw!7:n1Q&SU#epb>:<FbvG*V$N>3Hn.[&c++E1"

      And there are users:
         | name   | admin  | fullname  | ui              | uibits    | id                         | modifyDate    |
         | admin  | true   | Admin     | conversations   | 293853    | $admin   | Dec 4 2021    |
         | public | false  | Public    | conversations   | 7340036   | $public   | Apr 8 2021    |
         
      And there are groups:
         | name         | id           | modifyDate   |
         | Admin        | $Admin       | Apr 8 2021   |
         | Everybody    | $Everybody   | Apr 8 2021   |
         | Invited      | $Invited     | Apr 8 2021   |
         | Public       | $Public      | Apr 8 2021   |
         
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
         | name             | policy | id                | modifyDate |
         | Login Messages   | p1     | $Login Messages   | Jan 19 2018 |

      And the server has infos:
         | type            | text            |
         | upstreamMirror  | true    |

      And there are sites:
         | name      | headerTitle     | streamBgColor   |
         | main      | Downstream 6    | purple          |

      And the node service is started

   @javascript @downstream6
	Scenario: Setup downstream 6 server
      Then eventually there are 1 collections in the DB
