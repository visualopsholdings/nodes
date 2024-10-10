
export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test3

echo "downstream3 post count"
#node counts.js --fullUsers=2 --fullGroups=2 --fullStreams=2 --ideas=15 --fullSequences=2 --media=0 --formats=0 --policies=1
build/Counts --dbName=test3 --fullUsers=2 --fullGroups=2 --fullStreams=2 --ideas=5 --policies=1
[ "$?" != "0" ] && exit 1

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test2

echo "downstream3 -> downstream2 post count"
#node counts.js --ideas=15
build/Counts --dbName=test3 --ideas=5

