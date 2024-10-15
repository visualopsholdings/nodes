
# to be run in nodes

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test
export NODES_PUB_PORT=3012
export NODES_REP_PORT=3013

if [ "$1" != "noreset" ];
then
  ci/upstreamdb.sh test
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=test --users=4 --groups=2 --streams=4 --ideas=31 --docs=3 --media=2 --formats=2 --policies=4
  [ "$?" != "0" ] && exit 1
  
fi

export NODES_DATA_PORT=8810
export NODES_MSG_PORT=8811

./local.sh
