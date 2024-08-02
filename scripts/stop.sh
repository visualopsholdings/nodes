#!/bin/bash
# 
# Stop Nodes.

LIST=`ps -ef | grep "build/nodes" | grep -v grep | awk '{print $2}'`
if [ ! -z "$LIST" ]; then
	sudo kill -9 $LIST
fi

exit 0
