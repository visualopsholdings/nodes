
# to be run in nodes

export MONGO_PORT_27017_DB=test4
export NODES_PUB_PORT=3042
export NODES_REP_PORT=3043

if [ "$1" != "noreset" ];
then
  ci/downstreamdb4.sh
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=$MONGO_PORT_27017_DB --fullUsers=2 --fullGroups=4 --fullCollections=1 --objs=0 --policies=1
  [ "$?" != "0" ] && exit 1
fi

# upstream
export NODES_REMOTE_DATA_PORT=8810
export NODES_REMOTE_MSG_PORT=8811
export NODES_REMOTE_BIN_PORT=8812

# bind as upstream
export NODES_DATA_PORT=8840
export NODES_MSG_PORT=8841
export NODES_BIN_PORT=8842
export MEDIA_DIR=media4

if [ ! -d "$MEDIA_DIR" ];
then
  mkdir "$MEDIA_DIR"
fi

./local.sh
