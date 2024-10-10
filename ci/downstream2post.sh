
export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test2

#node counts.js --fullUsers=1 --fullGroups=1 --fullStreams=3 --ideas=10 --fullSequences=2 --media=2 --formats=2 --policies=2
build/Counts --dbName=test2 --fullUsers=1 --fullGroups=1 --fullStreams=3 --ideas=0 --policies=2

