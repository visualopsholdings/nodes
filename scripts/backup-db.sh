#!/bin/bash
# Server side DB backup script
#

FILENAME=vopsdb_`date "+%m%d%H%M%Y"`.tgz

rm -rf dump
mongodump --username=nodes --password=nodes --db=nodes

ERROR=$?
if [ $ERROR -ne 0 ]
then
	echo "error:" $ERROR
	exit 1
fi

tar czf $FILENAME dump

echo $FILENAME
