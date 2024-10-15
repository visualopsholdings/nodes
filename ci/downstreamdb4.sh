
# to be run in vops

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=test4

export NODE=false

bundle exec cucumber --tag "@downstream4" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
