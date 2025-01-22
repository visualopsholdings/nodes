
# to be run in nodes

export MONGO_PORT_27017_DB=test
export NODES_PUB_PORT=3012
export NODES_REP_PORT=3013

if [ "$1" != "noreset" ];
then
  ci/massiveupdb.sh
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=$MONGO_PORT_27017_DB --users=1 --groups=1 --collections=1 --objs=100000 --policies=1
  [ "$?" != "0" ] && exit 1
  
fi

export NODES_DATA_PORT=8810
export NODES_MSG_PORT=8811

./local.sh
