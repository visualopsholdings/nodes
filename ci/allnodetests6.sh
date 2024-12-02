
# execute ALL of the nodes tests.
#
# just run a test in a complex
#
#	upstream                  4ecaf2b6-42f2-49fe-b0ef-123758a3a4db (8810, 8811) (admin)
#		downstream <- testing   2d105350-8598-4377-b045-238194e23bc5
#		downstream2             14ee64a1-d68f-4f6f-862a-d1ea23210010 (8820, 8821) (initial sync) (translate)
#   downstream4 (mirror)    f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6 (8840, 8841) (mahsa)

echo "All node tests 6"
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
}

fail() {
	finish
	exit 1
}

# in case we run on a failed build.
finish

ci/run.sh upstream
[ "$?" != "0" ] && exit 1

ci/run.sh downstream2
[ "$?" != "0" ] && fail

ci/run.sh downstream4
[ "$?" != "0" ] && fail

ci/nodestest.sh "An obj can be moved from a non shared collection to a shared collection on upstream"
[ "$?" != "0" ] && fail

finish

exit 0
