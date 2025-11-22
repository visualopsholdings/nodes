# Nodes

The core subsystem for deliverying the JSON based objects, synchronizing nodes etc.

The Nodes system can be used to implement any "tree" of nodes that want to deliver arbitrary
JSON between them, synchronizing a MongoDB with that JSON:

Core to understanding how this might work you can look at this file which is used
to configure the "schema" that is used to maintain and synchronize objects:

```
[
  {
    "type": "user",
    "internal": true
  },
  {
    "type": "group",
    "internal": true
  },
  {
    "type": "policy",
    "coll": "policies",
    "nosync": true,
    "internal": true
  },
  {
    "type": "collection",
    "subobj": {
      "type": "obj",
      "field": "coll",
      "namefield": "text"
    }
  }
]
```

The "internal" objects are regarded as first class objects in the system and are used
to configure the security of the "collection" and "obj" objects by providing the idea
of editable policies.

A node can have an "upstream" which it communicates with using ECURVE cryptography (ZMQ)
and objects in the "collection" can be markes as "upstream" and then they will be synchronized
between nodes.

Nodes can also be a mirror of another node, and then they will be virtual copies of that node
at all times.

Because everything is implementred using message queuing, this system is perfect to be used in
a "DDIL" environment (Denied, Degraded, Interdicted, and Limited).

The best way to get started is to take a look at:

https://github.com/visualopsholdings/nodes-devops

## ZMQ API

You communicate with Nodes using the ZMQ message queues.

A PUB/SUB socket on port tcp://127.0.0.1:3012.

A REP/REQ socket on port tcp://127.0.0.1:3013.

[Here are the formats for the messages](src/msg/README.md)

## Building

The development process for all of this code used a normal Linux environment with the BOOST
libraries and a C++ compiler.

### Prerequisites

The best way to get started is to take a look at:

https://github.com/visualopsholdings/nodes-devops

And then find your particular OS and run those steps to get a basic binary-only build running 
for your platform. Once you have that you can come back in here and setup all the development
tools you might need.

Ok your back!!!!

This project can be found as the folder "nodes" in the thing you just setup. The next step
is to remove that folder and pull the SOURCE from git with:

```
git clone https://github.com/visualopsholdings/nodes.git
```

Then depending on your platform, add in necessary development tools:

On Ubuntu 24.04:

```
sudo apt-get update
sudo apt-get -y install g++ gcc make cmake ruby-rubygems ruby-bundler ruby-dev libssl-dev
```

On the maxc OS:

```
brew install cmake
```

For convenience, do all these inside a folder you can remove, they take a lot of disk space
but aren't used once installed.

```
mkdir working
cd working
```

#### Boost 1.85.0

if you can get this or later with a simple tool then do that, otherwise you will
need to build it from source:

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/BOOST.md)

#### CMake 3.30

if you can get this or later with a simple tool then do that, otherwise you will
need to build it from source:

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/CMAKE.md)

#### ZMQ

If you can get zeromq 4.3.5 or later, then use that otherwise you will need to build it
all from source.

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/ZMQ.md)

#### MongoDB

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/MONGO.md)

### This project

```
git clone https://github.com/visualopsholdings/nodes.git
nodes/scripts/build.sh
```

To run all the unit tests. And show failures.

```
cd nodes/build
make && make test || cat Testing/Temporary/LastTest.log 
```

## Testing

The end to end tests use cucumber (Ruby), to get that going:

```
bundle install
```

You will only need to do that once to install the ruby stuff.

```
./test.sh
```

For other projects you will need this in your .bashrc or equivalent

```
export NODES_HOME=/where/nodes/went
```

## More complicated tests

You can run tests that build multiple servers and run tests between then with:

```
ci/allnodetests.sh
```

## Testing dependencies

Testing relies on the external project:

https://github.com/visualopsholdings/nodes-test

If this changes, you can fetch it down and rebuild again with:

```
cd build
rm -rf _deps/nodes-test-* && make -j4
```

## Companion projects

### DevOps

https://github.com/visualopsholdings/nodes-devops

### A web front end

https://github.com/visualopsholdings/nodes-web

### An IRC front end

https://github.com/visualopsholdings/nodes-irc

## Current development focus

### Extensions to allow other types of messages (like binary)
### Really low end file system store instead of MongoDB

## License

Nodes is licensed under [version 3 of the GNU General Public License] contained in LICENSE.

## Acknowlegements

### ZeroMQ messaging

- https://zeromq.org/

### Boost C++ library

- https://www.boost.org/

### End to end testing

- https://cucumber.io/

### MongoDB

- https://www.mongodb.com
- https://mongocxx.org/api/current/annotated.html

## Change Log

### 22 May 2024
- Initial simple test implementation.

### 23 May 2024
- Start in on login msg and mongoDB access.

### 4 June 2024
- Write a general class for storage access and lot's of tests.

### 5 June 2024
- Introduce schemas and simplify storage access.

### 6 June 2024
- Add projections to mongo queries (select).

### 10 June 2024
- Tests correctly setup mongo data
- deleteMany() and insert() implemented for Mongo.

### 11 June 2024
- Almost get end to end chat working.

### 24 June 2024
- Implement VIDs and password checking for production (hash and salt in the DB)

### 26 June 2024
- Implement aggregation piplelines for security indexng.

### 28 June 2024
- Implement widthView and withEdit for queries.

### 30 June 2024
- Type system for DB data.

### 2 Jul 2024
- New FLTK admin interface (start of it) 

### 4 Jul 2024
- New RESTINIO based http daemon.

### 10 Jul 2024
- Finish basic login ready for testing.

### 12 Jul 2024
- Fixed bugs in login/admin and add Logoff.

### 19 Jul 2024
- Started in on "Chat" web app. Implemented streams.

### 23 Jul 2024
- Implemented "Conversation" in web app.

### 26 Jul 2024
- Move web app to https://github.com/visualopsholdings/zchttp

### 29 Jul 2024
- Rename project "nodes".

### 13 Aug 2024
- Implement upstream and going online to the upstream.

### 16 Aug 2024
- Implement upstream query of users and return.

### 23 Aug 2024
- Implement end to end discovery of users.
- implemented teams and adding a user to a team.

### 26 Aug 2024
- Users can be added and removed from a team
- A user can be have it's active and admin flags set.

### 28 Aug 2024
- Implement security correctly (me) for various messages that need it.

### 10 Sep 2024
- Add support for adding, deleting and editing team and streams.
- Add support for editing security.

### 11 Sep 2024
- Allow security policies to be edited.

### 12 Sep 2024
- First steps for creating share links.

### 13 Sep 2024
- Generate stream share tokens.

### 15 Sep 2024
- Remove all hard coded encryption keys. Generated when the server is reset.

### 16 Sep 2024
- Start in on creating new invited users.

### 17 Sep 2024
- Invitations working except for testing expiry.

### 18 Sep 2024
- expiry test

### 23 Sep 2024
- Add delete user.
- Fix bug with mulitple ADD UPSTREAM USER requests.

### 24 Sep 2024
- Switch to use "corr" (correlation id) instead of "socketid".

### 25 Sep 2024
- Add modification date testing.

### 27 Sep 2024
- Add collection changed testing.

### 1 Oct 2024
- Lot's and lot's of end to end tests.

### 3 Oct 2024
- First test for being an upstream working.

### 6 Oct 2024
- Implement user query message.

### 7 Oct 2024
- Nodes can be retrieved, added and deleted.

### 10 Oct 2024
- Groups can be upstream.

### 15 Oct 2024
- Lot's more syncing code and tests for such.

### 16 Oct 2024
- Move out the FLTK stuff to nodes-admin
- Synchorinise adds, add more tests.

### 20 Oct 2024
- More tests for synching ideas.

### 19 Nov 2024
- Implement delete by marking deleted objects and purging.
- for streams, teams and users

### 20 Nov 2024
- Implement delete by marking deleted objects and purging.
- for ideas

### 21 Nov 2024
- Refactor to use a "path" instead of a single "src".

### 22 Nov 2024
- Refactor to generalise streams and ideas ready for media and formats.

### 28 Nov 2024
- Refactor to generalise and to "collections" and "objs".

### 29 Nov 2024
- Implement "moveobject" message.

### 5 Dec 2024
- Add many more move tests.
- Implement ID lookup to cleanup all tests.

### 22 Dec 2024
- Make it really easy to remove logging or just log to stdout rather than
  rely on Boost.

### 27 Dec 2024
- Use nodes namespace everywhere
- Better exception handling for exceptions thrown from handlers.
- Start to remove internal dependancy on JSON.

### 30 Dec 2024
- Use "Data" as abstraction for the JSON code.

### 22 Jan 2024
- Add tests for much larger scale.
- Generating objs is now done in C++. Too slow in ruby.

### 24 Jan 2024
- rework to record last object dates for sub objects.

### 3 Feb 2025
- Do chunking (100 objects at a time)

### 4 Mar 2025
- Start in on binary build downloads.
- Start in on an FS version of the storage.
  - To use this, comment out set(USE_MONGO ON) in CMakeLists.txt but don't yet
    because it isn't implemented completely.

### 7 Nov 2025
- Start to use new "dict" project instead of boost json which
  allows real reflection in CPP.
- our new "Dict" is used on the "send" side now. boost::json still used in MongoDB and receiving.

### 10 Nov 2025
- Dict is used for a all of the MongoDB code now.

### 11 Nov 2025
- More Dict stuff.
- Get nodes-web working.

### 12 Nov 2025
- "Data" removed from all storage and security code.

### 13 Nov 2025
- "Data" removed completely.

### 15 Nov 2025
- "Json" and boost::json removed completely.

### 22 Nov 2025
- We now parse BSON directly to and from out Dict objects for the MongoDB work. So no JSON.




