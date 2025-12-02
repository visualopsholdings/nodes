
# to be run in nodes

export MONGO_PORT_27017_DB=test2
export NODES_PUB_PORT=3022
export NODES_REP_PORT=3023

if [ "$1" != "noreset" ];
then
  ci/downstreamdb2.sh
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=$MONGO_PORT_27017_DB --users=1 --groups=1 --collections=3 --objs=0 --policies=2
  [ "$?" != "0" ] && exit 1
fi

# upstream
export NODES_REMOTE_DATA_PORT=8810
export NODES_REMOTE_MSG_PORT=8811
export NODES_REMOTE_BIN_PORT=8812

# bind as upstream
export NODES_DATA_PORT=8820
export NODES_MSG_PORT=8821
export NODES_BIN_PORT=8822
export MEDIA_DIR=media2

if [ ! -d "$MEDIA_DIR" ];
then
  mkdir "$MEDIA_DIR"
fi

./local.sh
