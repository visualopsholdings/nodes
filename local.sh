
# Upstream is normal, downstream is test with port settings
# 
#	Terminal 1
# 		ci/upstreamdb.sh; ./downstreamdb.sh; ./downstreamdb2.sh; ./downstreamdb3.sh
# 		./localnodes.sh
#	or
# 		./local-ng.sh nodes
#
#	Terminal 2
# 		./local-other.sh test
# 
#	Terminal 3
# 		./local-other2.sh
# 
#	Terminal 4
# 		./local-other3.sh
# 
# Downstream is normal
# 
#	Terminal 1
# 		ci/upstreamdb.sh test; ./downstreamdb.sh main
# 		./localnodes.sh downstream
#	or
# 		./local-ng.sh nodes downstream
#
#	Terminal 2
# 		./local-other.sh test upstream
#
# New way:
#		./downstreamdb.sh main
#		ci/runupstream.sh 
#		./localnodes.sh downstream
#		Or
# 		./local-ng.sh nodes downstream
#		ci/killupstream.sh 

export LOG=trace

if [ "$MONGO_PORT_27017_DB" == "" ]; then
	export MONGO_PORT_27017_DB=dev
fi

if [ "$NODES_DATA_PORT" == "" ]; then
  build/nodes --test --logLevel=$LOG --dbName=$MONGO_PORT_27017_DB \
    --repPort=$NODES_REP_PORT --pubPort=$NODES_PUB_PORT
else
	if [ "$NODES_REMOTE_DATA_PORT" == "" ]; then
		export NODES_REMOTE_DATA_PORT=8810
	fi
	if [ "$NODES_REMOTE_MSG_PORT" == "" ]; then
		export NODES_REMOTE_MSG_PORT=8811
	fi
  build/nodes --test --logLevel=$LOG --dbName=$MONGO_PORT_27017_DB \
    --repPort=$NODES_REP_PORT --pubPort=$NODES_PUB_PORT \
	  --dataReqPort=$NODES_DATA_PORT --msgSubPort=$NODES_MSG_PORT \
	  --remoteDataReqPort=$NODES_REMOTE_DATA_PORT --remoteMsgSubPort=$NODES_REMOTE_MSG_PORT
fi

