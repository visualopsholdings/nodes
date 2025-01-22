
n=1
while [ 1 ];
do
  echo "Iteration $n"
  ci/allnodetests.sh
  [ "$?" != "0" ] && exit 1
  ((n++))
done  
