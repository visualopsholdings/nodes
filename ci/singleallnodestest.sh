
# just run a test in a complex
#
#	upstream
#		downstream <- testing
#		downstream2
#			downstream3
#   downstream4 (mirror)

finish() {
	ci/kill.sh upstream
	ci/kill.sh downstream2
	ci/kill.sh downstream3
	ci/kill.sh downstream4
}

fail() {
	finish
	exit 1
}

pushd ..
ci/run.sh upstream
[ "$?" != "0" ] && exit 1

ci/run.sh downstream2
[ "$?" != "0" ] && fail

ci/run.sh downstream3
[ "$?" != "0" ] && fail

ci/run.sh downstream4
[ "$?" != "0" ] && fail

ci/nodestest.sh "$1"
[ "$?" != "0" ] && fail

finish
popd

exit 0
