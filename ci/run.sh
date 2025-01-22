
if [ -f $1.pid ]; then
	echo "Already running $1. Kill first."
	exit 1
fi

echo "Running up $1"

ci/$1.sh > $1.log 2>&1 &
echo "$!" > $1.pid

if [ "$2" == "" ]; then
  timeout=10
else
  timeout=$2
fi

start=$(date +%s)
while [ 1 ]
do
	if [ "$1" == "upstream" ] || [ "$1" == "massiveup" ]; then
    cat $1.log | grep "init nodes"
    if [ "$?" == "0" ]; then
      echo "Ready."
      exit 0
    fi	
	fi
	cat $1.log | grep "error:"
	if [ "$?" == "0" ]; then
	  echo "failed with error"
	  exit 1
	fi
	cat $1.log | grep "Error: "
	if [ "$?" == "0" ]; then
	  echo "failed with error"
	  exit 1
	fi
	cat $1.log | grep "terminating"
	if [ "$?" == "0" ]; then
	  echo "failed with terminate"
	  exit 1
	fi
	cat $1.log | grep "Failing Scenarios:"
	if [ "$?" == "0" ]; then
	  echo "tests failed"
	  exit 1
	fi
	cat $1.log | grep "Discovery complete"
	if [ "$?" == "0" ]; then
		ci/$1post.sh
    [ "$?" != "0" ] && exit 1
		echo "Ready."
		exit 0
	fi
	now=$(date +%s)
	elapsed=$((now-start))
	if ((elapsed > timeout)); then
    echo "failed with timeout"
    exit 1
	fi
done
