#!/bin/bash

if [ $# -lt 2 ]
then
echo "Usage: ./logVideoIMURead <logfile> <videofile.avi>"
exit
fi

echo Running: cvlc $2
cvlc $2 &

echo Running: ../../bin/imuGX3 --LogReadFile $1
../../bin/imuGX3 --LogReadFile $1
