#!/bin/bash

if [ ! -f Shoreside.moos ]; then
	echo "Could not find mission configuration file for Shoreside community, execution unsuccessful, exiting...";
	exit;
else
	echo "Found mission configuration file for Shoreside community";
fi

if [ ! -f USV.moos ]; then
	echo "Could not find mission configuration file for USV community, execution unsuccessful, exiting...";
	exit;
else
	echo "Found mission configuration file for USV community";
fi

if [ ! -f USV.bhv ]; then
	echo "Could not find USV behavior file, execution unsuccessful, exiting...";
	exit;
else
	echo "Found USV behavior file";
fi

printf "Launching Shoreside MOOS Community \n"
pAntler Shoreside.moos >& /dev/null &
printf "Launching USV MOOS Community \n"
pAntler USV.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
mykill
printf "Done killing processes.   \n"