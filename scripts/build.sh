#!/bin/bash
# 
# Build the project

if [ ! -d "nodes" ];
then
  echo "nodes project missing."
  exit 1
fi

if [ ! -d "nodes/build" ];
then
  mkdir nodes/build
fi
pushd nodes/build
git pull
cmake ..
make -j4
popd
