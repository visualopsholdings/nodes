#!/bin/bash
# 
# Start ZMQCHAT

if [ ! -f "zmqchat/build/ZMQCHAT" ];
then
  echo "Need to build project first"
  exit 1
fi

./zmqchat/build/ZMQCHAT \
  --logLevel=trace \
  > zmqchat.log 2>&1 &

