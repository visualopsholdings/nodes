
# just run a test in a simple upstream -> downstream mode
# where upstream is running and fixed.

if [ ! -d "ci" ];
then
  echo "Run from root"
  exit 1
fi

finish() {
	ci/kill.sh upstream
}

fail() {
	finish
	exit 1
}

ci/run.sh upstream
[ "$?" != "0" ] && exit 1

ci/nodestest.sh "$1"
[ "$?" != "0" ] && fail

finish

exit 0
