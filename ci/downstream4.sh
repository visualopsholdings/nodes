
# to be run in nodes

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test4
export NODES_PUB_PORT=3042
export NODES_REP_PORT=3043

if [ "$1" != "noreset" ];
then
  ci/downstreamdb4.sh
  [ "$?" != "0" ] && exit 1

#  node counts.js --fullUsers=2 --fullGroups=4 --fullStreams=1 --ideas=0 --fullSequences=1 --media=0 --formats=0 --policies=1
  build/Counts --dbName=test4 --fullUsers=2 --fullGroups=4 --fullStreams=1 --ideas=0 --fullDocs=0 --media=0 --formats=0 --policies=1
  [ "$?" != "0" ] && exit 1
fi

export NODES_DATA_PORT=8850
export NODES_MSG_PORT=8851

./local.sh
