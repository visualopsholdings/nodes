#!/bin/bash
# 
# Run the server locally.
#
# MONGO_PORT_27017_DB     (dev)       the name of the DB containing the data.
# NODES_PUB_PORT          (3012)      the port number for MSG (pub)
# NODES_REP_PORT          (3013)      the port number for DATA (rep)
# NODES_DATA_PORT                     if specified the place to bind to for DATA
#
# If NODES_DATA_PORT has something
#   NODES_MSG_PORT                    the place to bind to for MSG
#   NODES_REMOTE_DATA_PORT  (8810)    the place to connect to upstream for DATA
#   NODES_REMOTE_MSG_PORT   (8811)    the place to connect to upstream for MSG
#
# info - general messages
# debug - outputs JOSN being transferred
# trace - deep debugging
export LOG=info

if [ "$MONGO_PORT_27017_DB" == "" ]; then
	export MONGO_PORT_27017_DB=dev
fi
if [ "$NODES_PUB_PORT" == "" ]; then
	export NODES_PUB_PORT=3012
fi
if [ "$NODES_REP_PORT" == "" ]; then
	export NODES_REP_PORT=3013
fi

if [ "$MEDIA_DIR" == "" ]; then
	export MEDIA_DIR=mediaout
  if [ ! -d "$MEDIA_DIR" ];
  then
    mkdir "$MEDIA_DIR"
  fi
fi

if [ "$MEDIA_DIR" != "" ]; then
	export MEDIA_DIR_SETTING="--mediaDir $MEDIA_DIR"
fi

if [ "$NODES_DATA_PORT" == "" ]; then
  build/nodes --test --logLevel=$LOG --dbName=$MONGO_PORT_27017_DB \
    --repPort=$NODES_REP_PORT --pubPort=$NODES_PUB_PORT \
    $MEDIA_DIR_SETTING
else
	if [ "$NODES_MSG_PORT" == "" ]; then
	  echo "Missing NODES_MSG_PORT"
		exit 1
	fi
	if [ "$NODES_REMOTE_DATA_PORT" == "" ]; then
		export NODES_REMOTE_DATA_PORT=8810
	fi
	if [ "$NODES_REMOTE_MSG_PORT" == "" ]; then
		export NODES_REMOTE_MSG_PORT=8811
	fi
  build/nodes --test --logLevel=$LOG --dbName=$MONGO_PORT_27017_DB \
    --repPort=$NODES_REP_PORT --pubPort=$NODES_PUB_PORT \
	  --dataReqPort=$NODES_DATA_PORT --msgSubPort=$NODES_MSG_PORT \
	  --remoteDataReqPort=$NODES_REMOTE_DATA_PORT --remoteMsgSubPort=$NODES_REMOTE_MSG_PORT \
    $MEDIA_DIR_SETTING
fi
