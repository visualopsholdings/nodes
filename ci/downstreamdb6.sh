
# to be run in nodes

export MONGO_PORT_27017_DB=test6
export NODE=false

bundle exec cucumber --tag "@downstream6" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
