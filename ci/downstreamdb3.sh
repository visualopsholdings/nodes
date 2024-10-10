
# to be run in nodes

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test3

export NODE=false

bundle exec cucumber --tag "@downstream3" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
