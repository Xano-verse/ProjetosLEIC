#!/bin/bash

# SCRIPT ARGUMENTS
IP=$1
PORT=$2

# includes 1 and 8
for i in {1..8}
do
	if [[ $i -lt 10 ]]; then
		echo "$IP $PORT $i" | nc tejo.tecnico.ulisboa.pt 59000 > report0$i.html
		echo "[32mcreated file report0$i.html[37m"
	else
		echo "$IP $PORT $i" | nc tejo.tecnico.ulisboa.pt 59000 > report$i.html
		echo "[32mcreated file report$i.html[37m"
	fi
done

