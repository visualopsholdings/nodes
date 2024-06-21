#!/bin/bash
# 
# Start ZMQCHAT

if [ ! -f "zmqchat/build/ZMQCHAT" ];
then
  echo "Need to build project first"
  exit 1
fi

./zmqchat/build/ZMQCHAT \
  --logLevel=trace \
  --dbConn=mongodb://fiveEstellas:visualops@127.0.0.1:27017/?authSource=fiveEstellas \
  --dbName=fiveEstellas \
  --certFile=/etc/letsencrypt/live/irc.visualops.com/privkey.pem \
  --chainFile=/etc/letsencrypt/live/irc.visualops.com/fullchain.pem \
  > zmqchat.log 2>&1 &
