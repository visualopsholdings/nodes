#!/bin/bash
# 
# Stop ZMQCHAT

LIST=`ps -ef | grep "ZMQCHAT" | grep -v grep | awk '{print $2}'`
if [ ! -z "$LIST" ]; then
	sudo kill -9 $LIST
fi

exit 0
