#!/bin/bash
# 
# Start Nodes.

if [ ! -f "nodes/build/nodes" ];
then
  echo "Need to install project first"
  exit 1
fi

if [ "$#" -lt 2 ]; 
then
	echo "usage: $0 dbname dbpass"
	exit 1
fi

DBNAME=$1
DBPASS=$2
HOSTNAME=$3

if [ "$#" -eq 4 ];
then
  MONGOS=$4
else
  MONGOS=127.0.0.1
fi

if [ -d nodes-lib ];
then
  export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib:`pwd`/nodes/build:`pwd`/nodes-lib:$LD_LIBRARY_PATH
  export DYLD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib:`pwd`/nodes/build:`pwd`/nodes-lib:$DYLD_LIBRARY_PATH
else
  export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
  export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
fi

if [ "$#" -eq 3 ]; 
then
  CERTS="--certFile=/etc/letsencrypt/live/$HOSTNAME/privkey.pem --chainFile=/etc/letsencrypt/live/$HOSTNAME/fullchain.pem"
  HOST="--hostName=$HOSTNAME"
fi

./nodes/build/nodes \
  --logLevel=debug \
  --dbConn=mongodb://$DBNAME:$DBPASS@$MONGOS:27017/?authSource=$DBNAME \
  --dbName=$DBNAME $CERTS $HOST \
  > nodes.log 2>&1 &
