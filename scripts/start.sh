#!/bin/bash
# 
# Start Nodes.

if [ ! -f "nodes/build/nodes" ];
then
  echo "Need to build project first"
  exit 1
fi

LD_LIBRARY_PATH=/usr/local/lib:/home/nodes/nodes/build:/home/nodes/nodes-lib ./nodes/build/nodes \
  --logLevel=trace \
  --dbConn=mongodb://fiveEstellas:visualops@127.0.0.1:27017/?authSource=fiveEstellas \
  --dbName=fiveEstellas \
  --certFile=/etc/letsencrypt/live/irc.visualops.com/privkey.pem \
  --chainFile=/etc/letsencrypt/live/irc.visualops.com/fullchain.pem \
  > nodes.log 2>&1 &
