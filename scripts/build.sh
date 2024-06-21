#!/bin/bash
# 
# Build the project

if [ ! -d "zmqchat" ];
then
  echo "zmqchat project missing."
  exit 1
fi

if [ ! -d "zmqchat/build" ];
then
  mkdir zmqchat/build
fi
pushd zmqchat/build
git pull
cmake ..
make
make test
popd
