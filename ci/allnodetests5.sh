
# execute ALL of the nodes tests.
#
# just run a test in a complex
#
#	upstream                  4ecaf2b6-42f2-49fe-b0ef-123758a3a4db (8810, 8811) (admin)
#		downstream <- testing   2d105350-8598-4377-b045-238194e23bc5
#		downstream2             14ee64a1-d68f-4f6f-862a-d1ea23210010 (8820, 8821) (initial sync) (translate)
#     downstream5 (mirror)  15a7bfe2-6065-4c1f-926f-8bcf192c84c0 (8850, 8851)
#   downstream4 (mirror)    f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6 (8840, 8841) (mahsa)
#     downstream6 (mirror)  05fd392d-1092-4255-8323-38c774e4f6a8 (8860, 8861)

echo "All node tests 5"
date "+%H:%M:%S"

if [ ! -d "ci" ];
then
  echo "Run from root"
  exit 1
fi

finish() {
	ci/kill.sh upstream
	ci/kill.sh downstream2
	ci/kill.sh downstream4
	ci/kill.sh downstream5
	ci/kill.sh downstream6
}

fail() {
	finish
	exit 1
}

# in case we run on a failed build.
finish

# test moves

# ci/run.sh upstream
# [ "$?" != "0" ] && exit 1
# 
# ci/run.sh downstream2
# [ "$?" != "0" ] && fail
# 
# ci/run.sh downstream4
# [ "$?" != "0" ] && fail
# 
# ci/run.sh downstream5
# [ "$?" != "0" ] && fail
# 
# ci/run.sh downstream6
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "An obj can be moved from a shared collecion to a non shared collection on downstream"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "An obj can be moved between shared collections"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "An obj can be moved from a shared collection to a non shared collection on mirror"
# [ "$?" != "0" ] && fail
# 
# finish

exit 0
