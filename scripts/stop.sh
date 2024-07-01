#!/bin/bash
# 
# Stop zmqchat

LIST=`ps -ef | grep "zmqchat" | grep -v grep | awk '{print $2}'`
if [ ! -z "$LIST" ]; then
	sudo kill -9 $LIST
fi

exit 0
