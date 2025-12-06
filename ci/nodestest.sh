
export MONGO_PORT_27017_DB=dev

# bind as an upstream
export NODES_DATA_PORT=8800
export NODES_MSG_PORT=8801

# local data
export NODES_PUB_PORT=3112
export NODES_REP_PORT=3113

export NODE=false

bundle exec cucumber -n "$1" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
