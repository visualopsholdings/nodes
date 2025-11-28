
# to be run in nodes

export MONGO_PORT_27017_DB=test3
export NODES_PUB_PORT=3032
export NODES_REP_PORT=3033

if [ "$1" != "noreset" ];
then
  ci/downstreamdb3.sh
  [ "$?" != "0" ] && exit 1

  echo "downstream3 pre count"
  build/Counts --dbName=$MONGO_PORT_27017_DB --users=2 --groups=2 --collections=2 --objs=5 --policies=1
  [ "$?" != "0" ] && exit 1
fi

# upstream
export NODES_REMOTE_DATA_PORT=8820
export NODES_REMOTE_MSG_PORT=8821

# bind as upstream
export NODES_DATA_PORT=8830
export NODES_MSG_PORT=8831
export MEDIA_DIR=media3

if [ ! -d "$MEDIA_DIR" ];
then
  mkdir "$MEDIA_DIR"
fi

./local.sh
