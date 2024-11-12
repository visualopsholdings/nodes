
# execute ALL of the nodes tests.
#
# just run a test in a complex
#
#	upstream                  4ecaf2b6-42f2-49fe-b0ef-123758a3a4db (8810, 8811) (admin)
#		downstream <- testing   2d105350-8598-4377-b045-238194e23bc5
#   downstream4 (mirror)    f2ecaf81-943d-4ffd-a3d4-fc2d2d48c1c6 (8840, 8841) (mahsa)

echo "All node tests 4"
date "+%H:%M:%S"

if [ ! -d "ci" ];
then
  echo "Run from root"
  exit 1
fi

finish() {
	ci/kill.sh upstream
	ci/kill.sh downstream4
}

fail() {
	finish
	exit 1
}

# in case we run on a failed build.
finish

# ci/run.sh upstream
# [ "$?" != "0" ] && exit 1
# 
# ci/run.sh downstream4
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "A new draft doc created on downstream4 appears on upstream"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "An idea can be moved from one stream to another on a mirror"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "A certificate can be requested from the upstream"
# [ "$?" != "0" ] && fail
# 
# ci/nodestest.sh "A new media is reflected in a mirror"
# [ "$?" != "0" ] && fail
# 
# finish

exit 0
