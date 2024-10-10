
# to be run in nodes

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test2
export NODES_PUB_PORT=3022
export NODES_REP_PORT=3023

if [ "$1" != "noreset" ];
then
  ci/downstreamdb2.sh
  [ "$?" != "0" ] && exit 1

#  node counts.js --users=1 --groups=1 --streams=3 --ideas=0 --sequences=2 --media=2 --formats=1 --policies=0
  build/Counts --dbName=test2 --users=1 --groups=1 --streams=3 --ideas=0 --policies=2
  [ "$?" != "0" ] && exit 1
fi

export NODES_DATA_PORT=8830
export NODES_MSG_PORT=8831

./local.sh
