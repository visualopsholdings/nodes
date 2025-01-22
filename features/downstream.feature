@nodes
Feature: Downstream Test

   Background:
      And there are users:
         | fullname           | id        | upstream  | password  | modifyDate   | admin |
         | Waiting discovery  | $tracy    | true      | no        | 1 Jan 1970   | true  |
         
       When there are users:
         | name      | admin  | fullname  | email           | ui              | uibits    |
         | alice     | false  | Alice     | alice@fe.com    | conversations   | 293853    |
         
      And there are groups:
         | name               | id                          | upstream  | modifyDate   |
         | Waiting discovery  | $Team 1    | true      | 1 Jan 1970   |
         
      And there are groups:
         | name        |
         | Team 3      |
         
      And there are users in group "Team 3":
         | name      | roles          |
         | alice     | user     |

      And there are policies:
         | name   | viewuser           | viewgroup | edituser           | editgroup | execuser           | execgroup |
         | p3     |                    | Team 3    |                    | Team 3    |                    | Team 3    |
         
      And there are collections:
         | name               | id                    | upstream  | modifyDate   |
         | Waiting discovery  | $Collection 1         | true      | 1 Jan 1970   |
         | Waiting discovery  | $Shared Collection    | true      | 1 Jan 1970   |

      And there are collections:
         | name          | policy |
         | Collection 4  | p3     |

      And the server has id "2d105350-8598-4377-b045-238194e23bc5"
      And the server has privateKey "sM[0CH.io/U2[jvLMbB@UEU4Z]lTX@EDG2#DD7UK"
      And the server has pubKey "/9AxT<](&2X.HceOX#VoJ.zONJ#=*lgYu8hKgP*4"
      And the server has upstream "localhost"
      And the server has upstreamPubKey "31r]#CW@p}=]xYRwvg+P>NrDffBG}wXL0%t:[j6("
      
      And there are sites:
         | name      | headerTitle     | streamBgColor   |
         | main      | Downstream 1    | lightgreen      |
 
      And the node service is started
#      And the DB is new
      And she waits 1 seconds

   @javascript @downstream
  Scenario: Setup downstream server
      Then eventually there are 3 collections in the DB
      
   @javascript
   Scenario: An existing user can be pulled from upstream
      When she sends add user from upstream with "$leanne"
      And she receives ack
      And eventually the user fullname "Leanne" with salt and hash appears in the DB

   @javascript
   Scenario: An existing user can pulled be through 2 upstreams
     When she sends add user from upstream with "$leanne" to downstream 2
      And she receives ack
      And she sends user "$leanne" to downstream 2
      And she receives user "Waiting discovery"
      And she waits 1 seconds
      And she sends user "$leanne" to downstream 2
      And she receives user "Leanne"
      And she sends add user from upstream with "$leanne" to downstream 3
      And she receives ack
      And she sends user "$leanne" to downstream 3
      And she receives user "Waiting discovery"
      And she waits 1 seconds
      And she sends user "$leanne" to downstream 3
      Then she receives user "Leanne"

   @javascript
   Scenario: An existing group can be pulled from upstream
     When she sends add group from upstream with "$Team 2"
      And she receives ack
      And eventually the group "Team 2" with 1 member appears in the DB

   @javascript
   Scenario: Downstreams have synced
      When she sends collections
      Then she receives 3 collections
      When she sends objs for "Collection 1" as "tracy"
      Then she receives 500 objs

      When she sends collections to downstream 2
      Then she receives 3 collections
      When she sends objs for "$Shared Collection" to downstream 2
      Then she receives 106 objs
      
      When she sends collections to downstream 3
      Then she receives 2 collections
      When she sends objs for "$Shared Collection" to downstream 3
      Then she receives 106 objs

      When she sends collections to downstream 4
      Then she receives 5 collections
      When she sends objs for "$Collection 1" to downstream 4
      Then she receives 500 objs
      When she sends objs for "$Collection 2" to downstream 4
      Then she receives 1 objs
      When she sends objs for "$Shared Collection" to downstream 4
      Then she receives 101 objs

   @javascript
   Scenario: Massive servers are synched
      When she sends collections
      Then she receives 3 collections
      When she sends objs for "Shared Collection" as "tracy"
      Then she receives 100000 objs

  @javascript
   Scenario: Change to a user is reflected in the downstream servers
   
      When she sends set user fullname of "tracy" to "Joan"
      
      And she sends user "$tracy" to upstream
      And she receives user "Joan"
      
      And she sends set user fullname of "$tracy" to "Sue" to upstream
      And she waits 1 seconds
      
      And she sends user with id "$tracy"
      And she receives user "Sue"

      And she sends user "$tracy" to downstream 2
      And she receives user "Sue"
      
      And she sends user "$tracy" to downstream 3
      And she receives user "Sue"
      
      And she sends user "$tracy" to downstream 4
      And she receives user "Sue"

  @javascript
   Scenario: Change to a group is reflected in the downstream servers
   
      When she sends set group name of "Team 1" to "New Group"
      
      And she sends group "$Team 1" to upstream
      And she receives group "New Group"

      And she sends set group name of "$Team 1" to "Another Group" to upstream
      And she waits 1 seconds

      And she sends group with id "$Team 1"
      And she receives group "Another Group"

      And she sends group "$Team 1" to downstream 2
      And she receives group "Another Group"
      
      And she sends group "$Team 1" to downstream 3
      And she receives group "Another Group"
      
      And she sends group "$Team 1" to downstream 4
      And she receives group "Another Group"
 
    @javascript
   Scenario: Change to a collection is reflected in the downstream servers
   
      When she sends set collection name of "Shared Collection" to "Collection x"

      And she sends collection "$Shared Collection" to upstream
      And she receives collection "Collection x"

      And she sends set collection name of "$Shared Collection" to "Collection y" to upstream
      And she waits 1 seconds

      And she sends collection with id "$Shared Collection"
      And she receives collection "Collection y"

      And she sends collection "$Shared Collection" to downstream 2
      And she receives collection "Collection y"
      
      And she sends collection "$Shared Collection" to downstream 3
      And she receives collection "Collection y"
      
      And she sends collection "$Shared Collection" to downstream 4
      And she receives collection "Collection y"

    @javascript
   Scenario: A new collection can be created on downstream 5

      When she sends add collection "New Conversation" as "$admin" to downstream 5
     And she sends collections to downstream 5
      Then she receives 5 collections

      # 5 is a mirror of upstream2
     And she sends collections to downstream 2
      Then she receives 4 collections
      
      # upstream shouldn't get it.
     And she sends collections to upstream
      Then she receives 4 collections

    @javascript
   Scenario: A new collection created on upstream appears on downstream mirrors

      When she sends collections to downstream 4
      And she receives 5 collections
     
      And she sends add collection "New Collection" as "$tracy" to upstream
      And she sends collections to upstream
      And she receives 5 collections
      
      # wait for the downstreams 2 sync
      And she waits 1 seconds
      
      And she sends collections to downstream 4
      And she receives 6 collections
     
      And she sends collections to downstream 6
      Then she receives 6 collections
     
    @javascript
   Scenario: A new group created on upstream appears on downstream mirrors

     When she sends groups to downstream 4
      And she receives 6 groups
     
      And she sends add group "New Group" as "$tracy" to upstream
     And she sends groups to upstream
      And she receives 3 groups
       
     And she sends groups to downstream 4
      And she receives 7 groups
     
     And she sends groups to downstream 6
      Then she receives 7 groups

  @javascript
   Scenario: A collection created in a mirror downstream 4 is reflected in the upstream
   
      When she sends add collection "New Collection" as "$admin" to downstream 4
     And she sends collections to downstream 4
      And she receives 6 collections

     And she sends collections to upstream
      Then she receives 5 collections

    @javascript
   Scenario: A new group created on downstream 4 appears on upstream

      When she sends add group "New Group" as "$admin" to downstream 4
     And she sends groups to downstream 4
      And she receives 7 groups

     And she sends groups to upstream
      Then she receives 3 groups

  @javascript
   Scenario: A collection created in a mirror downstream 6 is reflected in the upstream
   
      When she sends add collection "New Collection" as "$tracy" to downstream 6
     And she sends collections to downstream 6
      And she receives 6 collections

     And she sends collections to upstream
      And she receives 5 collections

     And she sends collections to downstream 4
      Then she receives 6 collections

    @javascript
   Scenario: A new group created on downstream 6 appears on upstream

      When she sends add group "New Group" as "$tracy" to downstream 6
     And she sends groups to downstream 6
      And she receives 7 groups

     And she sends groups to upstream
      And she receives 3 groups

     And she sends groups to downstream 4
      Then she receives 7 groups

   @javascript
   Scenario: An existing collection can be pulled from upstream
   
      When she sends add collection from upstream with "$Collection 2"
      And she receives ack
      And eventually the collection "Collection 2" has 1 objs in the DB
      
     And she sends collections
      And she receives 4 collections

      And she sends obj "My Obj" as "tracy" to "Collection 2"
      And she sends objs for "Collection 2" as "tracy"
      And she receives 2 objs

     And she sends objs for "$Collection 2" to upstream
      Then she receives 2 objs

  @javascript
   Scenario: A new collection with objs can be pulled from upstream
   
      # add a new collection to the upstream with 2 new objs.
      When she sends add collection "New Collection" as "$tracy" to upstream
      And she saves the result
      And she sends obj "My Obj 1" as "$tracy" to saved collection to upstream
      And she sends obj "My Obj 2" as "$tracy" to saved collection to upstream
      And she sends objs for saved collection to upstream
      And she receives 2 objs
      
      # make sure nothing came down.
      When she sends collections
      And she receives 3 collections
      Then eventually there are 601 objs in the DB
      
      # get the collection from upstream.
      When she sends add collection from upstream with saved collection
      And she receives ack
      And she sends collections
      And she receives 4 collections
      Then eventually there are 603 objs in the DB
      
      # add a new obj to this upstream collection.
      When she sends obj "I know" as "tracy" to "New Collection"
      Then eventually the collection "New Collection" has 3 objs in the DB
      
      When she sends objs for saved collection to upstream
      Then she receives 3 objs

   @javascript
   Scenario: A deleted obj is reflected upstream and downstream servers
   
      When she sends objs for "Shared Collection" as "tracy"
      And she receives 101 objs
	   And she sends delete obj "Obj 5" as "tracy" in "Shared Collection"
      And she sends objs for "Shared Collection" as "tracy"
      And she receives 100 objs

      And she sends objs for "$Shared Collection" to upstream
      And she receives 100 objs

      And she sends objs for "$Shared Collection" to downstream 2
      And she receives 105 objs

      And she sends objs for "$Shared Collection" to downstream 3
      And she receives 105 objs

      And she sends objs for "$Shared Collection" to downstream 4
      And she receives 100 objs

   @javascript
   Scenario: An obj can be moved from one collection to another on a mirror
   
      # downstream 4 is a mirror of upstream
      # old collection is Collection 2
      And she sends objs for "$Collection 2" as "$tracy" to downstream 4
      And she receives 1 objs
      # new collection is Shared Collection
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      
      # move Count 1
      When she sends move obj "$Count 1" to "$Shared Collection" as "$tracy" to downstream 4
      And she receives ack
      
      And she sends objs for "$Collection 2" as "$tracy" to downstream 4
      And she receives 0 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 102 objs

      # reflected in the upstream
      And she sends objs for "$Collection 2" as "$tracy" to upstream
      And she receives 0 objs
      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 102 objs

   @javascript
   Scenario: An obj can be moved from a shared collection to a non shared collection on downstream
   
      And she sends objs for "Shared Collection" as "tracy"
      And she receives 101 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 101 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 101 objs

      # downstream2
      # old collection is Shared Collection
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 101 objs
      # new collection is Collection 4
      And she sends objs for "$Collection 4" as "$tracy" to downstream 2
      And she receives 0 objs

      # move Shared Obj 1
      When she sends move obj "$Shared Obj 1" to "$Collection 4" as "$tracy" to downstream 2
      And she receives ack
      
      # wait for downstreams to catch up
      And she waits 1 seconds
      
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 100 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 2
      And she receives 1 objs

      # downstream4
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 100 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 4
      And she receives 1 objs

      # downstream5
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 100 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 5
      And she receives 1 objs

      # downstream6
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 100 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 6
      And she receives 1 objs

      # upstream
      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 100 objs
      And she sends objs for "$Collection 4" as "$tracy" to upstream
      And she receives 1 objs

      # move Shared Obj 1 back
      When she sends move obj "$Shared Obj 1" to "$Shared Collection" as "$tracy" to downstream 2
      And she receives ack

      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 101 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 2
      And she receives 0 objs

      # downstream4
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 4
      And she receives 0 objs

      # downstream5
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 101 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 5
      And she receives 0 objs

      # downstream6
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 101 objs
      And she sends objs for "$Collection 4" as "$tracy" to downstream 6
      And she receives 0 objs

      # upstream
      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 101 objs
      And she sends objs for "$Collection 4" as "$tracy" to upstream
      And she receives 0 objs
      
   @javascript
   Scenario: An obj can be moved from a shared collection to a non shared collection on mirror
   
      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 101 objs
      And she sends objs for "$Collection 2" as "$tracy" to upstream
      And she receives 1 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 101 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 2
      And she receives 0 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 101 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 5
      And she receives 0 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 101 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 6
      And she receives 1 objs

       # downstream4
      # old collection is Shared Collection
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      # new collection is Collection 2
      And she sends objs for "$Collection 2" as "$tracy" to downstream 4
      And she receives 1 objs

      # move Shared Obj 1
      When she sends move obj "$Shared Obj 1" to "$Collection 2" as "$tracy" to downstream 4
      And she receives ack

      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 4
      And she receives 2 objs

      # upstream
      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to upstream
      And she receives 2 objs

      # downstream 2
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 2
      And she receives 1 objs

      # downstream 5
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 5
      And she receives 1 objs

      # downstream 6
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 6
      And she receives 2 objs
      
   @javascript
   Scenario: An obj can be moved between shared collections
   
      # Shared 2 Collection
      And she sends objs for "$Shared 2 Collection" as "$tracy" to upstream
      And she receives 0 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 4
      And she receives 0 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 5
      And she receives 0 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 6
      And she receives 0 objs

      # downstream2
      # old collection is Shared Collection
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 101 objs
      # new collection is Shared 2 Collection
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 2
      And she receives 0 objs

      # move Shared Obj 1
      When she sends move obj "$Shared Obj 1" to "$Shared 2 Collection" as "$tracy" to downstream 2
      And she receives ack

      # wait for downstreams to catch up
      And she waits 1 seconds
      
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 100 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 2
      And she receives 1 objs

      # downstream4
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 100 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 4
      And she receives 1 objs

      # downstream5
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 100 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 5
      And she receives 1 objs

      # downstream6
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 100 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 6
      And she receives 1 objs

      # move Shared Obj 1 back
      When she sends move obj "$Shared Obj 1" to "$Shared Collection" as "$tracy" to downstream 2
      And she receives ack

      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 101 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 2
      And she receives 0 objs

      # downstream4
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 4
      And she receives 0 objs

      # downstream5
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 5
      And she receives 101 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 5
      And she receives 0 objs

      # downstream6
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 6
      And she receives 101 objs
      And she sends objs for "$Shared 2 Collection" as "$tracy" to downstream 6
      And she receives 0 objs
      
   @javascript
   Scenario: An obj can be moved from a non shared collection to a shared collection on upstream
   
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 101 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 2
      And she receives 0 objs
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 101 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 4
      And she receives 1 objs

      # upstream old collection is Shared Collection
      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 101 objs
      # new collection is Collection 2
      And she sends objs for "$Collection 2" as "$tracy" to upstream
      And she receives 1 objs

      # move Shared Obj 1
      When she sends move obj "$Shared Obj 1" to "$Collection 2" as "$tracy" to upstream
      And she receives ack

      And she sends objs for "$Shared Collection" as "$tracy" to upstream
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to upstream
      And she receives 2 objs

      # downstream2
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 2
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 2
      And she receives 1 objs

      # downstream4
      And she sends objs for "$Shared Collection" as "$tracy" to downstream 4
      And she receives 100 objs
      And she sends objs for "$Collection 2" as "$tracy" to downstream 4
      And she receives 2 objs
