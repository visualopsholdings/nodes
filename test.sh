#!/bin/bash
# 
# Run the cucumber tests

export MONGO_PORT_27017_DB=dev
export NODES_PUB_PORT=3012
export NODES_REP_PORT=3013

bundle exec cucumber $1  --tags "not (@nodes or @broken)" -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
