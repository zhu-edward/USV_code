#!/bin/bash

if [ $# -lt 1 ]
then
echo "Usage: ./logVideoIMUWrite <videofile.avi>"
exit
fi

echo Running: pAntler imuSpatial.moos
pAntler imuSpatial.moos &

echo Running: ./recordVideo $1
./recordVideo $1

pkill pAntler

cd logs
newLogFolder=$(ls -t | head -n 1)
cd ..

mv $1 logs/$newLogFolder


