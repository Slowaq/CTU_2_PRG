#!/bin/sh

HW=03b
PROGRAM=./b3b36prg-hw$HW-genref

echo "Generate and solve bonus assignments"

mkdir -p files
for i in `seq 1 4`
do
   PROBLEM=`printf "files/hw%s-$i" $HW`
   echo "Generate random input '$PROBLEM.in'"
   $PROGRAM -generate > $PROBLEM.in 2>/dev/null
   echo "Solve '$PROBLEM.in' and store the reference solution to '$PROBLEM.out'"
   $PROGRAM < $PROBLEM.in > $PROBLEM.out 2>$PROBLEM.err
done

return 0
