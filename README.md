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

## Companion projects

https://github.com/visualopsholdings/zmqchat

## Current development focus

### Reference version in NodeJS

## License

ZMQChat is licensed under [version 3 of the GNU General Public License] contained in LICENSE.



