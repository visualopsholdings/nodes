
# to be run in nodes

export MONGO_PORT_27017_DB=test6
export NODES_PUB_PORT=3062
export NODES_REP_PORT=3063

if [ "$1" != "noreset" ];
then
  ci/downstreamdb6.sh
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=$MONGO_PORT_27017_DB --fullUsers=2 --fullGroups=4 --fullStreams=1 --ideas=0 --fullDocs=0 --media=0 --formats=0 --policies=1
  [ "$?" != "0" ] && exit 1
fi

# upstream
export NODES_REMOTE_DATA_PORT=8840
export NODES_REMOTE_MSG_PORT=8841

# bind as upstream
export NODES_DATA_PORT=8860
export NODES_MSG_PORT=8861

./local.sh
