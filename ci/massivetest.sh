
# execute the massive upstream test
#
# just run a test in a complex
#

echo "Massive upstream tests"
date "+%H:%M:%S"

if [ ! -d "ci" ];
then
  echo "Run from root"
  exit 1
fi

finish() {
	ci/kill.sh massiveup
}

fail() {
	finish
	exit 1
}

# in case we run on a failed build.
finish

ci/run.sh massiveup 30
[ "$?" != "0" ] && exit 1

ci/nodestest.sh "Massive servers are synched"
[ "$?" != "0" ] && fail

finish

exit 0
