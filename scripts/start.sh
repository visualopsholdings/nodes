#!/bin/bash
# 
# Start Nodes.

if [ ! -f "nodes/build/nodes" ];
then
  echo "Need to install project first"
  exit 1
fi

if [ -d /home/nodes/nodes-lib ];
then
  export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib:/home/nodes/nodes/build:/home/nodes/nodes-lib:$LD_LIBRARY_PATH
else
  export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
fi

./nodes/build/nodes \
  --logLevel=trace \
  --dbConn=mongodb://fiveEstellas:visualops@127.0.0.1:27017/?authSource=fiveEstellas \
  --dbName=fiveEstellas \
  --certFile=/etc/letsencrypt/live/irc.visualops.com/privkey.pem \
  --chainFile=/etc/letsencrypt/live/irc.visualops.com/fullchain.pem \
  > nodes.log 2>&1 &
