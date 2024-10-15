
echo "downstream3 post count"
build/Counts --dbName=test3 --fullUsers=2 --fullGroups=2 --fullStreams=2 --ideas=15 --fullDocs=1 --media=0 --formats=0 --policies=1
[ "$?" != "0" ] && exit 1

echo "downstream3 -> downstream2 post count"
build/Counts --dbName=test2 --ideas=15
