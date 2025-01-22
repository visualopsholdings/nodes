
echo "downstream3 post count"
build/Counts --dbName=test3 --fullUsers=2 --fullGroups=2 --fullCollections=2 --objs=106 --policies=1
[ "$?" != "0" ] && exit 1

echo "downstream3 -> downstream2 post count"
build/Counts --dbName=test2 --objs=106
