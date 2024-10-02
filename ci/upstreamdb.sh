
# to be run in nodes

export MONGO_PORT_27017_TCP_ADDR=127.0.0.1
export MONGO_PORT_27017_TCP_PORT=27017

if [ "$1" == "test" ]; then
	export MONGO_PORT_27017_DB=test
else
	export MONGO_PORT_27017_DB=dev
fi

export NODE=false

bundle exec cucumber --tag "@upstream" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
