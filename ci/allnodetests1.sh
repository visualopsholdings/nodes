
# execute ALL of the nodes tests.
#
# just run a test in a complex
#
#	upstream                  4ecaf2b6-42f2-49fe-b0ef-123758a3a4db (8810, 8811) (admin)
#		downstream <- testing   2d105350-8598-4377-b045-238194e23bc5
#		downstream2             14ee64a1-d68f-4f6f-862a-d1ea23210010 (8820, 8821) (initial sync) (translate)
#			downstream3           e4448086-8a22-4db0-9dcf-c95a56418d77 (8830, 8831) (initial sync, extra ideas)
#     downstream5 (mirror)  15a7bfe2-6065-4c1f-926f-8bcf192c84c0 (8850, 8851)
#   downstream4 (mirror)    f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6 (8840, 8841) (mahsa)

echo "All node tests 1"
date "+%H:%M:%S"

finish() {
	ci/kill.sh upstream
	ci/kill.sh downstream2
	ci/kill.sh downstream3
  ci/kill.sh downstream4
# 	ci/kill.sh downstream5
}

fail() {
	finish
	exit 1
}

# test all of those that can be run together
pushd ..

# in case we run on a failed build.
finish

ci/run.sh upstream
[ "$?" != "0" ] && exit 1

# ci/nodestest.sh "A build can be downloaded"
# [ "$?" != "0" ] && fail

ci/nodestest.sh "An existing user can be pulled from upstream"
[ "$?" != "0" ] && fail

ci/nodestest.sh "An existing group can be pulled from upstream"
[ "$?" != "0" ] && fail

# ci/nodestest.sh "An existing doc can be pulled from upstream"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "An existing media can be pulled from upstream"
# [ "$?" != "0" ] && fail

ci/run.sh downstream2
[ "$?" != "0" ] && fail

ci/run.sh downstream3
[ "$?" != "0" ] && fail

ci/run.sh downstream4
[ "$?" != "0" ] && fail

ci/nodestest.sh "Downstreams have synced"
[ "$?" != "0" ] && fail

ci/nodestest.sh "Change to a user is reflected in the downstream servers"
[ "$?" != "0" ] && fail

# ci/nodestest.sh "Change to a team is reflected in the downstream servers"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "Change to a sequence is reflected in the downstream servers"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "Change to a stream is reflected in the downstream servers"
# [ "$?" != "0" ] && fail

finish

# ci/run.sh upstream
# [ "$?" != "0" ] && exit 1
# 
# ci/run.sh downstream2
# [ "$?" != "0" ] && fail
# 
# ci/run.sh downstream5
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "A new stream can be created on downstream5"
# [ "$?" != "0" ] && fail
# 
# finish

popd

exit 0
