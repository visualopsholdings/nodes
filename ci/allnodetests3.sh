
# execute ALL of the nodes tests.
#
# just run a test in a complex
#
#	upstream                  4ecaf2b6-42f2-49fe-b0ef-123758a3a4db (8810, 8811) (admin)
#		downstream <- testing   2d105350-8598-4377-b045-238194e23bc5
#   downstream4 (mirror)    f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6 (8840, 8841) (mahsa)
#     downstream6 (mirror)  05fd392d-1092-4255-8323-38c774e4f6a8 (8860, 8861)

echo "All node tests 3"
date "+%H:%M:%S"

if [ ! -d "ci" ];
then
  echo "Run from root"
  exit 1
fi

finish() {
	ci/kill.sh upstream
	ci/kill.sh downstream4
	ci/kill.sh downstream6
}

fail() {
	finish
	exit 1
}

# in case we run on a failed build.
finish

ci/run.sh upstream
[ "$?" != "0" ] && exit 1

ci/run.sh downstream4
[ "$?" != "0" ] && fail

ci/run.sh downstream6
[ "$?" != "0" ] && fail

ci/nodestest.sh "A collection created in a mirror downstream 4 is reflected in the upstream"
[ "$?" != "0" ] && fail

ci/nodestest.sh "A new group created on downstream 4 appears on upstream"
[ "$?" != "0" ] && fail

finish

exit 0
