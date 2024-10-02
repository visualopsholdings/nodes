
export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017
export MONGO_PORT_27017_DB=dev
export NODES_DATA_PORT=8820
export NODES_MSG_PORT=8821
export NODES_PUB_PORT=3112
export NODES_REP_PORT=3113

export NODE=false

bundle exec cucumber -n "$1" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
