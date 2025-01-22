
# to be run in nodes

export MONGO_PORT_27017_DB=test
export NODE=false

bundle exec cucumber --tag "@massiveup" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
