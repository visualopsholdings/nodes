
# to be run in nodes

export MONGO_PORT_27017_DB=test
export NODES_PUB_PORT=3012
export NODES_REP_PORT=3013

if [ "$1" != "noreset" ];
then
  ci/upstreamdb.sh
  [ "$?" != "0" ] && exit 1

  build/Counts --dbName=$MONGO_PORT_27017_DB --users=4 --groups=2 --collections=4 --objs=605 --policies=4
  [ "$?" != "0" ] && exit 1
  
fi

export NODES_DATA_PORT=8810
export NODES_MSG_PORT=8811
export MEDIA_DIR=media1

if [ ! -d "$MEDIA_DIR" ];
then
  mkdir "$MEDIA_DIR"
fi

rm -rf $MEDIA_DIR/*
cp test/media/eeeeeee-ffff-gggg-hhhhhhhh $MEDIA_DIR
cp test/media/aaaaaaaa-bbbb-cccc-dddddddd $MEDIA_DIR
cp test/media/llllllll-mmmm-nnnn-oooooooo $MEDIA_DIR

./local.sh
