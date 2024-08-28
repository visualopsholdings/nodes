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
  "me": "user guid"
}
```

We process and return;

```
{ 
  "type": "streams", 
  "streams": [
    {
      "name": "Conversation 1",
      "stream": "stream guid",
      "policy": "stream policy guid"
    }
  ]
}
```

#### Stream

When this is received:

```
{ 
  "type": "stream", 
  "stream": "stream guid",
  "me": "user guid"
}
```

We process and return;

```
{ 
  "type": "streams, 
  "stream": ""stream guid",
  "stream": {
    "name": "Conversation 1",
    "stream": "stream guid",
    "policy": "stream policy guid"
  }
}
```

#### Policy users

Since the Nodes system is persistent, when you join a stream, you can query the 
users in that stream by the stream policy and join those users too.

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

This is sent to the REQ socket when a message happens on IRC, and will be received on the 
SUB socket when a new message is received.

```
{ 
  "type": "message", 
  "text": "Message text",
  "stream": "stream guid",
  "policy": "stream policy guid",
  "user": "user guid"
}
```

#### Users

When this is received:

```
{ 
  "type": "users"
}
```

We process and return;

```
{ 
  "type": "users", 
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

#### User

When this is received:

```
{ 
  "type": "user", 
  "user": "user guid"
}
```

We process and return;

```
{ 
  "type": "user, 
  "user": "user guid",
  "user": {
    "name": "tracy",
    "fullname": "Tracy",
    "id": "user guid"
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
  "socketid": "a socketid sent through and passed back"
}
```

We process and return Ack or an error.

This will send a message to the upstream server, and when the result is available it will be 
sent on the SUB port as a "queryResult". The socketid is passed through and is a value understood
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
#### Groups

When this is received:

```
{ 
  "type": "groups", 
  "me": "user guid"
}
```

We process and return;

```
{ 
  "type": "groups", 
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

#### Group

When this is received:

```
{ 
  "type": "group",
  "group": "group guid", 
  "me": "user guid"
}
```

We process and return;

```
{ 
  "type": "group", 
  "group": {
    "name": "Team 1",
    "id": "group guid"
  }
}
```

#### Members

A group contains members, and this message will query for these and polulate the 
full name of the member.

When this is received:

```
{ 
  "type": "members",
  "group": "group guid"
}
```

We process and return;

```
{ 
  "type": "members", 
  "members": [
    {
      "user": "user guid"
      "fullname": "The fuilname of the user"
    }
  ]
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
  "msg": "LOL what are you doing!"
}
```
