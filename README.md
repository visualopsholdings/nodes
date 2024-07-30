# Nodes

A system for delivering JSON messages amongst a tree of servers using ZMQ.

## Visual Ops

Visual Ops is a messaging system (and more) that exists on the edge. Visual Ops nodes are
standalone servers which can be formed into a tree of servers delivering new messages to users
using ZMQ message queues and (in the web) Web sockets.

- There is a full featured Web App which can be configured as a standalone app on iOS and Android
and any device that can run a browser.
- Messages persist in a MongoDB database.
- A security system allows subsets of users to View and Edit "Streams" which are analagous to Channels
in IRC.
- A system called "Nodes" correctly delivers missing messages when nodes "go away".
- The system uses ZMQ message queues in a novel fashion which are the backbone for delivery
and use EC (Eliptical Curve) Cryptography to ensure transport security.
- Visual Ops is monolithic. Every server is completely standalone and require no more resources
from the internet (Cloud).

For these reasons, it would be the perfect backend for IRC Clients, or other messaging systems and solve many of the existing drawbacks
and problems that plague IRC etc.

This project implements an open source implementation of this in C++.

### ZMQ API

You communicate with Nodes using the ZMQ message queues.

A PUB/SUB socket on port tcp://127.0.0.1:3012.

A REP/REQ socket on port tcp://127.0.0.1:3013.

[Here are the formats for the messages](src/msg/README.md)

## Building

The development process for all of this code used a normal Linux environment with the BOOST
libraries and a C++ compiler.

So on your Linux (or mac using Homebrew etc), get all you need:

```
sudo apt-get update
sudo apt-get -y install g++ gcc make cmake ruby-rubygems
```

### Prerequisites

For convenience, do all these inside a folder you can remove, they take a lot of disk space
but aren't used once installed.

```
mkdir working
cd working
```

#### Boost 1.85.0

if you can get this or later with a simple tool then do that, otherwise you will
need to build it from source:

```
wget https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-b2-nodocs.tar.gz
tar xzf boost-1.85.0-b2-nodocs.tar.gz 
cd boost-1.85.0
./bootstrap.sh --prefix=/usr --with-python=python3
./b2 stage threading=multi link=shared boost.stacktrace.from_exception=off

as root user:
./b2 install threading=multi link=shared
```

#### ZMQ

If you can get zeromq 4.3.5 or later, then use that otherwise you will need to build it
all from source.

```
wget https://download.libsodium.org/libsodium/releases/libsodium-1.0.19-stable.tar.gz  
tar xzf libsodium-1.0.19-stable.tar.gz
cd libsodium-stable
./configure
make 
sudo make install
sudo ldconfig
cd ..

wget https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.tar.gz
tar xzf zeromq-4.3.5.tar.gz
cd zeromq-4.3.5
./configure
make
sudo make install
sudo ldconfig
cd ..

git clone https://github.com/zeromq/libzmq.git
cd libzmq
mkdir build
cd build
cmake ..
make -j4
make test
sudo make install
cd ../..

git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
cmake ..
make -j4
sudo make install
cd ../..
cd ..
```

#### MongoDB

On OS X or Linux:

```
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.1/mongo-cxx-driver-r3.10.1.tar.gz
tar xzf mongo-cxx-driver-r3.10.1.tar.gz
cd mongo-cxx-driver-r3.10.1/build
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBSONCXX_POLY_USE_BOOST=1                      \
    -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
cmake --build . -j4
sudo cmake --build . --target install
cd ../..
```

On a linux you might need:

```
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

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

Now this project:

```
git clone https://github.com/visualopsholdings/nodes.git
nodes/scripts/build.sh
```

To run all the unit tests. And show failures.

```
make && make test || cat Testing/Temporary/LastTest.log 
```

## Testing

The end to end tests use cucumber (Ruby), to get that going:

```
./test.sh
```

## Companion projects

### A web front end

https://github.com/visualopsholdings/nodes-web

### An IRC front end

https://github.com/visualopsholdings/nodes-irc

## Current development focus

### Implement chat web app in nodes-web.

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



