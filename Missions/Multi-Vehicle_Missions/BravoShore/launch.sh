#!/bin/bash

if [ ! -f Shoreside.moos ]; then
	echo "Could not find mission configuration file for Shoreside community, execution unsuccessful, exiting...";
	exit;
else
	echo "Found mission configuration file for Shoreside community";
fi

printf "Launching Shoreside MOOS Community \n"
pAntler Shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
mykill
printf "Done killing processes.   \n"