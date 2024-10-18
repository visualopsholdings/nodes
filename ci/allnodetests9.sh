
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
#     downstream6 (mirror)  05fd392d-1092-4255-8323-38c774e4f6a8 (8860, 8861)

echo "All node tests 9"
date "+%H:%M:%S"

finish() {
	ci/kill.sh upstream
	ci/kill.sh downstream2
	ci/kill.sh downstream3
}

fail() {
	finish
	exit 1
}

# test all of those that can be run together
pushd ..

# ci/run.sh upstream
# [ "$?" != "0" ] && exit 1
# 
# ci/run.sh downstream2
# [ "$?" != "0" ] && fail
# 
# ci/run.sh downstream3
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "An existing user can be pushed downstream"
# [ "$?" != "0" ] && fail
# 
# finish

popd

exit 0
