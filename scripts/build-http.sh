#!/bin/bash
# 
# Build the project for http access

if [ ! -d "zmqchat" ];
then
  echo "zmqchat project missing."
  exit 1
fi

pushd zmqchat/angular/login-ui
npm install
./build.sh
popd

pushd zmqchat/angular/admin-ui
npm install
./build.sh
popd

pushd zmqchat/angular/chat-ui
npm install
./build.sh
popd
