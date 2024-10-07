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

if [ -d nodes-lib ];
then
  export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib:`pwd`/nodes/build:`pwd`/nodes-lib:$LD_LIBRARY_PATH
  export DYLD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib:`pwd`/nodes/build:`pwd`/nodes-lib:$DYLD_LIBRARY_PATH
else
  export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
  export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
fi

if [ "$#" -gt 2 ]; 
then
  CERTS="--certFile=/etc/letsencrypt/live/$3/privkey.pem --chainFile=/etc/letsencrypt/live/$3/fullchain.pem"
  HOST="--hostName=$3"
  if [ "$#" -eq 4 ]; 
  then
    BIND="--bindAddress=$4"
  fi
fi

if [ "$#" -eq 5 ];
then
  MONGOS=$5
else
  MONGOS=127.0.0.1
fi

./nodes/build/nodes \
  --logLevel=trace \
  --dbConn=mongodb://$DBNAME:$DBPASS@$MONGOS:27017/?authSource=$DBNAME \
  --dbName=$DBNAME $CERTS $HOST $BIND \
  --dataReqPort=8810 --msgSubPort=8811 \
  > nodes.log 2>&1 &
