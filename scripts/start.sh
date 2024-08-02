#!/bin/bash
# 
# Start Nodes.

if [ ! -f "nodes/build/nodes" ];
then
  echo "Need to install project first"
  exit 1
fi

if [ "$#" -ne 2 ]; 
then
	echo "usage: $0 dbname hostname"
	exit 1
fi

DBNAME=$1
HOSTNAME=$2

if [ -d /home/nodes/nodes-lib ];
then
  export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib:/home/nodes/nodes/build:/home/nodes/nodes-lib:$LD_LIBRARY_PATH
else
  export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
fi

./nodes/build/nodes \
  --logLevel=trace \
  --dbConn=mongodb://$DBNAME:visualops@127.0.0.1:27017/?authSource=$DBNAME \
  --dbName=$DBNAME \
  --certFile=/etc/letsencrypt/live/$HOSTNAME/privkey.pem \
  --chainFile=/etc/letsencrypt/live/$HOSTNAME/fullchain.pem \
  > nodes.log 2>&1 &
