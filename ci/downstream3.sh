
# to be run in nodes

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test3
export NODES_PUB_PORT=3032
export NODES_REP_PORT=3033

if [ "$1" != "noreset" ];
then
  ci/downstreamdb3.sh
  [ "$?" != "0" ] && exit 1

  echo "downstream3 pre count"
  build/Counts --dbName=test3 --users=2 --groups=2 --streams=2 --ideas=5 --docs=1 --media=0 --formats=0 --policies=1
  [ "$?" != "0" ] && exit 1
fi

export NODES_DATA_PORT=8840
export NODES_MSG_PORT=8841
export NODES_REMOTE_DATA_PORT=8830
export NODES_REMOTE_MSG_PORT=8831

./local.sh
