
# to be run in nodes

export MONGO_PORT_27017_DB=test5
export NODES_PUB_PORT=3052
export NODES_REP_PORT=3053

if [ "$1" != "noreset" ];
then
  ci/downstreamdb5.sh
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=$MONGO_PORT_27017_DB --fullUsers=2 --fullGroups=4 --fullCollections=1 --objs=0 --policies=1
  [ "$?" != "0" ] && exit 1
fi

# upstream
export NODES_REMOTE_DATA_PORT=8820
export NODES_REMOTE_MSG_PORT=8821

# bind as upstream
export NODES_DATA_PORT=8850
export NODES_MSG_PORT=8851

./local.sh
