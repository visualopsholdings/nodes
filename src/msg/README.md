# Nodes - Messages

This is a document describing each of the messages that can be sent to Nodes and the reply 
you would get back for each.

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

The session id, is just turned around for convenience. It isn't used by Nodes.

#### Streams

When this is received:

```
{ 
  "type": "streams", 
  "me": "user guid",
  "test": {
    "time": 123456
  }
}
```

If the time is smaller than the internal time then we return that the collection was changed:

```
{ 
  "type": "streams", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "streams",
  "test": {
    "time": 123456
  },
  "streams": [
    {
      "name": "Conversation 1",
      "stream": "stream guid",
      "policy": "stream policy guid"
    }
  ]
}
```

The idea for a platform using this is to remember "time" in some way and then turn
it around in the requests and then if it is smaller than the internal time
that the collection was changed they can use the cached version of the collection.

This presents "micro hammering" of this service and only returns new data when a collection has changed.

#### Stream

When this is received:

```
{ 
  "type": "stream", 
  "stream": "stream guid",
  "me": "user guid",
  "test": {
    "time": "UTC time"
  }
}
```

If the time is earlier than the modifyDate of the stream then we return.

```
{ 
  "type": "stream", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "stream", 
  "stream": "stream guid",
  "stream": {
    "name": "Conversation 1",
    "stream": "stream guid",
    "policy": "stream policy guid",
    "modifyDate": "UTC time"
  }
}
```

#### Set Stream

When this is received:

```
{
  "type": "setstream",
  "id": "stream guid",
  "name": "A new name",
  "me": "userid guid"
}
```

If the user can edit the stream the details are set and Ack is returned.

#### Add Stream

When this is received:

```
{
  "type": "addstream",
  "name": "My new stream",
  "me": "userid guid"
}
```

We find a policy just for "me" and create a new stream with that policy and return Ack.

#### Delete Stream

When this is received:

```
{
  "type": "deletestream",
  "id": "stream guid",
  "me": "userid guid"
}
```

If the user is allowed to edit the stream they can delete it. If succesful we return "ack".

#### Set Stream Policy

When this is received:

```
{
  "type":"setstreampolicy",
  "add": [
    { "context":"user", "type":"edit", "id":"66e0246b62df890f877fc1d8" },
    { "context":"user", "type":"view", "id":"66e0246b62df890f877fc1d8" },
    { "context":"user", "type":"exec", "id":"66e0246b62df890f877fc1d8" }
  ],
  "remove": [
    "//accesses/1/groups/0",
    "//accesses/0/groups/0"
  ]
}
```

The remove is VERY similar to https://datatracker.ietf.org/doc/html/rfc6901
except for the extra "/" at the start of each line.

If the user can edit the stream the policy is modified, searched for and then 
set and Ack is returned.

#### Stream Share Link

Get a share link to a stream

When this is received:

```
{ 
  "type": "streamsharelink", 
  "stream": "stream guid", 
  "group": "group guid",
  "expires": 4,
  "me": "user guid"
}
```

We process and return;

```
{ 
  "type": "streamsharelink", 
  "url": "http://www.google.com"
}
```

#### Can Register

Test a registration token (from the share link)

When this is received:

```
{ 
  "type": "canreg", 
  "token": "share link token"
}
```

We process and return;

```
{ 
  "type": "canreg",
  "newVopsID": "a new VID",
  "canRegister": true,
  "requireFullname": true,
}
```

#### Policy users

Since the Nodes system is persistent, when you join a stream, you can query the 
users in that stream by the stream policy and join those users too.

When this is received:

```
{ 
  "type": "policyusers", 
  "policy": "policy guid" 
}
```

We process and return;

```
{ 
  "type": "policyusers", 
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

#### Policy groups

Since the Nodes system is persistent, when you join a stream, you can query the 
groups in that stream by the stream policy and join those users too.

When this is received:

```
{ 
  "type": "policygroups", 
  "policy": "policy guid" 
}
```

We process and return;

```
{ 
  "type": "policygroups", 
  "groups": [
    {
      "name": "Team 1",
      "id": "group guid"
    },
    {
      "name": "Team 2",
      "id": "group guid"
    }
  ]
}
```

#### Message

This is sent to the REQ socket when a message happens on IRC, and will be received on the 
SUB socket when a new message is received.

```
{ 
  "type": "message", 
  "text": "Message text",
  "stream": "stream guid",
  "policy": "stream policy guid",
  "me": "user guid",
  "corr": "a correlation id sent through and passed back"
}
```

#### Users

When this is received:

```
{ 
  "type": "users",
  "test": {
    "time": 123456
  }
}
```

If the time is smaller than the internal time then we return that the collection was changed:

```
{ 
  "type": "users", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "users", 
  "test": {
    "time": 123456
  },
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

The idea for a platform using this is to remember "time" in some way and then turn
it around in the requests and then if it is smaller than the internal time
that the collection was changed they can use the cached version of the collection.

This presents "micro hammering" of this service and only returns new data when a collection has changed.

#### User

When this is received:

```
{ 
  "type": "user", 
  "user": "user guid"
  "test": {
    "time": "UTC time"
  }
}
```

If the time is earlier than the modifyDate of the user then we return:

```
{ 
  "type": "user", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "user, 
  "user": "user guid",
  "user": {
    "name": "tracy",
    "fullname": "Tracy",
    "id": "user guid",
    "modifyDate": "UTC time"
  }
}
```

#### Query

To add a new user from an upstream server, you can use this message first to send a query
to the server for it to return a list of users that might match.

When this is received:

```
{ 
  "type": "query",
  "objtype": "user",
  "email": "xxxx",
  "corr": "a correlation id sent through and passed back"
}
```

We process and return Ack or an error.

This will send a message to the upstream server, and when the result is available it will be 
sent on the SUB port as a "queryResult". The correlation id is passed through and is a value understood
by the system which might maintain a list of sockets (like websockets) to communicate back to 
the relevant user.

#### New User

You can add a new user to the system from an upstream server by using this message.

When this is received:

```
{ 
  "type": "newuser",
  "id": "user guid",
  "upstream": true
}
```

We process and return Ack or an error.

If this message is successful, we run a "discovery" on the upstream server to populate the
user details.

#### Search Users

You can find users by providing a string that might match the fullname of the user.

When this is received:

```
{ 
  "type": "searchusers",
  "q": "xxxx"
}
```

We process and return;

```
{ 
  "type": "searchusers, 
  "result": [
    {
      "name": "tracy",
      "fullname": "Tracy",
      "id": "user guid"
    }
  ]
}
```

#### Delete User

When this is received:

```
{
  "type": "deleteuser",
  "id": "user guid"
}
```

If succesful we return "ack".

#### Groups

When this is received:

```
{ 
  "type": "groups", 
  "me": "user guid",
  "test": {
    "time": 123456
  }
}
```

If the time is smaller than the internal time then we return that the collection was changed:

```
{ 
  "type": "groups", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "groups", 
  "test": {
    "time": 123456
  },
  "groups": [
    {
      "name": "Team 1",
      "id": "group guid"
    },
    {
      "name": "Team 2",
      "id": "group guid"
    }
  ]
}
```

The idea for a platform using this is to remember "time" in some way and then turn
it around in the requests and then if it is smaller than the internal time
that the collection was changed they can use the cached version of the collection.

This presents "micro hammering" of this service and only returns new data when a collection has changed.

#### Group

When this is received:

```
{ 
  "type": "group",
  "group": "group guid", 
  "me": "user guid",
  "test": {
    "time": "UTC time"
  }
}
```

If the time is earlier than the modifyDate of the group then we return.

```
{ 
  "type": "group", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "group", 
  "group": {
    "name": "Team 1",
    "id": "group guid",
    "modifyDate": "UTC time"
  }
}
```

#### Add Group

When this is received:

```
{
  "type": "addgroup",
  "name": "My new team",
  "me": "userid guid"
}
```

We find a policy just for "me" and create a new group with that policy and return Ack.

#### Set Group

When this is received:

```
{
  "type": "setgroup",
  "id": "group guid",
  "name": "A new name",
  "me": "userid guid"
}
```

If the user can edit the group the details are set and Ack is returned.

#### Delete Group

When this is received:

```
{
  "type": "deletegroup",
  "id": "group guid",
  "me": "userid guid"
}
```

If the user is allowed to edit the group they can delete it. If succesful we return "ack".

#### Members

A group contains members, and this message will query for these and polulate the 
full name of the member.

When this is received:

```
{ 
  "type": "members",
  "group": "group guid",
  "me": "user guid",
  "test": {
    "time": "UTC time"
  }
}
```

If the time is earlier than the modifyDate of the group then we return.

```
{ 
  "type": "members", 
  "test": {
    "latest": true
  }
}
```

Otherwise we process and return;

```
{ 
  "type": "members", 
  "group": {
    "members": [
      {
        "user": "user guid"
        "fullname": "The fuilname of the user"
      }
    ],
    "modifyDate": "UTC time"
  }
}
```

#### New Member

Use this message to add a new member to a group.

When this is received:

```
{ 
  "type": "newmember",
  "group": "group guid",
  "id": "user guid"
}
```

We process and return Ack or an error.

#### Delete Member

Use this message to remove a member from a group.

When this is received:

```
{ 
  "type": "deletemember",
  "group": "group guid",
  "id": "user guid"
}
```

We process and return Ack or an error.

#### Ideas

"Ideas" are what we call the "messages" that are persisted in the system.

When this is received:

```
{ 
  "type": "ideas",
  "stream": "stream guid"
}
```

We process and return;

```
{ 
  "type": "ideas", 
  "ideas": [
    {
      "text": "Message text",
      "stream": "stream guid",
      "policy": "stream policy guid",
      "user": "user guid",
      "id": "group guid"
    }
  ]
}
```

#### Policy

"Policy"s are attached to each object to let the system know if a user or group can see,
edit or execute things against that object.

When this is received:

```
{ 
  "type": "policy",
  "objtype": "stream",
  "id": "object guid"
}
```

We process and return;


```
{ 
  "type": "policy", 
  "policy": [
    {
      "path": "//accesses/0/groups/0",
      "type": "view",
      "context": "group",
      "id": "5ef34b9a7264281391b4c72e",
      "name": "Public"
    },
    {
      "path": "//accesses/0/groups/1",
      "type": "view",
      "context": "group",
      "id": "601aa304d7a55b33de48707a",
      "name": "Everybody"
    },
    {
      "path": "//accesses/1/groups/0",
      "type": "edit",
      "context": "group",
      "id": "601aa304d7a55b33de48707a",
      "name": "Everybody"
    },
    {
      "path": "//accesses/2/groups/0",
      "type": "exec",
      "context": "group",
      "id": "601aa304d7a55b33de48707a",
      "name": "Everybody"
    }
  ]
}
```

If you take a look at the GUI in nodes-web this format will make sense :-)

#### Infos

"Infos" are name=value pairs used to describe the status of a server. Infos that exist
include the id of the server, it's various keys, the last time it synced etc.

When this is received:

```
{ 
  "type": "infos"
}
```

We process and return;

```
{ 
  "type": "infos", 
  "infos": [
    {
      "type": "serverId"
      "text": "The server id"
    }
  ]
}
```

#### Set Info

An info value can be set with this message. For the time being the only setting understood
is for resetting the server.

When this is received:

```
{ 
  "type": "setinfo",
  "serverId": "none"
}
```

We allocate a NEW server id, and then set the upstream and keys to an upstream server that is
valid and return Ack.

#### Certs

```
{ 
  "type": "certs"
}
```

This will be processed by the Visual Ops system and it will return:

```
{ 
  "type": "certs", 
  "ssl": true,
  "cettFile": "xxxx",
  "chainFile": "yyy"
}
```

If there isn't ant certificate file, it will send back:

```
{ 
  "type": "certs", 
  "ssl": false
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
  "level": "warning",
  "msg": "LOL what are you doing!"
}
```
