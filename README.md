# ZMQChat

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
- A system called "ZMQChat" correctly delivers missing messages when nodes "go away".
- The system uses ZMQ message queues in a novel fashion which are the backbone for delivery
and use EC (Eliptical Curve) Cryptography to ensure transport security.
- Visual Ops is monolithic. Every server is completely standalone and require no more resources
from the internet (Cloud).

For these reasons, it would be the perfect backend for IRC Clients and solve many of the existing drawbacks
and problems that plague IRC

This project will implement an open source implementation of this in C++.

### ZMQ messages

A PUB socket on port tcp://127.0.0.1:3012.

A REP socket on port tcp://127.0.0.1:3013.

### JSON formats

#### Login

When this is received:

```
{ 
  "type": "login", 
  "session": "internal session id",
  "password": "password" 
}
```

We process and return;

```
{ 
  "type": "user", 
  "name": "tracy",
  "fullname": "Tracy",
  "id": "user guid",
  "session": "internal session id"
}
```

The session id, is just turned around for convenience. It isn't used by ZMQChat.

#### Streams

When this is received:

```
{ 
  "type": "streams", 
  "user": "user guid",
}
```

We process and return;

```
{ 
  "type": "streams", 
  "user": "user guid",
  "streams": [
    {
      "name": "Conversation 1",
      "stream": "stream guid",
      "policy": "stream policy guid"
    }
  ]
}
```

#### Policy users

When this is received:

```
{ 
  "type": "policyusers", 
  "policy": "stream policy guid" 
}
```

We process and return;

```
{ 
  "type": "user", 
  "name": "policyusers",
  "id": "stream policy guid",
  "users": [
    {
      "name": "tracy",
      "fullname": "Tracy",
      "id": "user guid"
    },
    {
      "name": "leanne",
      "fullname": "Leanne",
      "id": "user guid"
    }
  ]
}
```

#### Message

When this is received:

```
{ 
  "type": "message", 
  "text": "Message text",
  "stream": "stream guid",
  "policy": "stream policy guid",
  "user": "user guid"
}
```

#### Acknowlege

This is received on the REP socket when a Message is sent.

```
{ 
  "type": "ack"
}
```

#### Error

This is received on the REP socket when a bad request has happened

```
{ 
  "type": "err",
  "msg": "LOL what are you doing!"
}
```

## Building

The development process for all of this code used a normal Linux environment with the BOOST
libraries and a C++ compiler.

So on your Linux (or mac using Homebrew etc), get all you need:

```
sudo apt-get update
sudo apt-get -y install g++ gcc make cmake
```

### Prerequisites

For convenience, do all these inside a folder you can remove, they take a lot of disk space
but aren't used once instsalled.

```
mkdir working
cd working
```

Boost 1.85.0 if you can get this or later with a simple tool then do that, otherwise you will
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

ZMQ Stuff.

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
cd ..

git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..
```

MongoDB.

```
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.1/mongo-cxx-driver-r3.10.1.tar.gz
tar xzf mongo-cxx-driver-r3.10.1.tar.gz
cd mongo-cxx-driver-r3.10.1/build
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBSONCXX_POLY_USE_BOOST=1                      \
    -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
cmake --build .
sudo cmake --build . --target install
cd ../..
```

Now this project:

```
git clone https://github.com/visualopsholdings/zmqchat.git
cd zmqchat
mkdir build
cd build
cmake ..
make
make test
```

To run all the unit tests. And show failures.

```
make && make test || cat Testing/Temporary/LastTest.log 
```

## Testing

The end to end tests use cucumber (Ruby), to get that going:

```
$ bundle install
$ bundle exec cucumber
```

## Companion projects

https://github.com/visualopsholdings/zmqirc

## Current development focus

### Implement login message

## License

ZMQChat is licensed under [version 3 of the GNU General Public License] contained in LICENSE.

## Acknowlegements

### ZeroMQ messaging

- https://zeromq.org/

### Boost C++ library

- https://www.boost.org/

### End to end testing

- https://cucumber.io/

## Change Log

### 22 May 2024
- Initial simple test implementation.

### 23 May 2024
- Start in on login msg and mongoDB access.

### 4 June 2024
- Write a general class for storage access and lot's of tests.

### 5 June 2024
- Introduce schemas and simplify storage access.
