#!/bin/bash

for i in $(seq 1 7);
do
	time python3 projeto.py < teste$i.in
done
