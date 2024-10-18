
# just run a test in a simple upstream -> downstream mode
# where upstream is running and fixed.

finish() {
	ci/kill.sh upstream
}

fail() {
	finish
	exit 1
}

pushd ..
ci/run.sh upstream
[ "$?" != "0" ] && exit 1

ci/nodestest.sh "$1"
[ "$?" != "0" ] && fail

finish
popd

exit 0
