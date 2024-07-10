#!/bin/bash
# 
# Start ZMQCHAT HTTP

if [ ! -f "zmqchat/build/zchttp" ];
then
  echo "Need to build project first"
  exit 1
fi

./zmqchat/build/zchttp \
  --logLevel=trace \
  > zchttp.log 2>&1 &
