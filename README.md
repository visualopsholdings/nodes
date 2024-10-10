# Nodes

The core subsystem for deliverying the messages, synchronizing nodes etc.

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

No this project can be found as the folder nodes-web in the thing you just setup. The next step
is to remove that folder and pull the SOURCE from git with:

```
git clone https://github.com/visualopsholdings/nodes.git
```

Then depending on your platform, add in necessary development tools:

On Ubuntu 24.04:

```
sudo apt-get update
sudo apt-get -y install g++ gcc make cmake ruby-rubygems libssl-dev
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

#### ZMQ

If you can get zeromq 4.3.5 or later, then use that otherwise you will need to build it
all from source.

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/ZMQ.md)

#### MongoDB

[Instructions](https://github.com/visualopsholdings/nodes-devops/blob/main/dev/MONGO.md)
On OS X or Linux:

#### FLTK

We build some simple GUIs for admin, and for that we use FLTK.

On OS X:

```
git clone https://github.com/fltk/fltk.git
cd fltk
mkdir build
cd build
cmake ..
make
sudo make install
```

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

You will need a machine with a display for this, so don't try to run it headless (for now). It
also probably won't run in Windows with the WSL since you will need to run all of the tests
from the Windows side and install Ruby etc there.

For other projects you will need this in your .bashrc or equivalent

```
export NODES_HOME=/where/nodes/went
```

## More complicated tests

You can run tests that build multiple servers and run tests between then with.

```
cd ci
/allnodetests1.sh
```

## Testing dependencies

Testing relies on the external project:

https://github.com/visualopsholdings/nodes-test

If this changes, you can fetch it down and rebuild again with:

```
cd build
rm -rf _deps/nodes-test-*
make
```

## Companion projects

### DevOps

https://github.com/visualopsholdings/nodes-devops

### A web front end

https://github.com/visualopsholdings/nodes-web

### An IRC front end

https://github.com/visualopsholdings/nodes-irc

## Current development focus

### Connecting and synchronizing with upstream servers. 

For now you can only make a Visual Ops (closed source) server an upstream and you can only pull down users 
from that server.

All of teams, users, streams and policies should all be able to be synchronized, and Nodes (OSS)
should be able to be an upstream server.

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

### FLTK

- https://www.fltk.org/doc-1.4

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





