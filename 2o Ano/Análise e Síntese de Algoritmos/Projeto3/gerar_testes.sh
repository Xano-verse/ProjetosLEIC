#!/bin/bash

n=10
m=5
t=50
variance=3
max_cap=10
max_req=10

: '
for i in $(seq 1 20);
do
	python3 gera3.py $(($n + i * 10)) $(($m+i*10)) $(($t+i*50)) $(($variance+i*10)) $(($max_cap+i*10)) $max_req > teste$i.in 

	while [ $(python3 projeto.py < teste$i.in) = "-1" ];
	do
		python3 gera3.py $(($n + i * 10)) $(($m+i*10)) $(($t+i*50)) $(($variance+i*10)) $(($max_cap+i*10)) $max_req > teste$i.in 
		echo "not yet $i";
	done
done
'


for i in $(seq 1 20);
do
	python3 grafico_apenasgerador.py $(($n+i*10)) $(($m+i*5)) $(($t+i*50)) > teste$i.in 

	while [ $(python3 projeto.py < teste$i.in) = "-1" ];
	do
		python3 grafico_apenasgerador.py $(($n + i * 10)) $(($m+i*5)) $(($t+i*50)) > teste$i.in 
		echo "not yet $i";
	done
done


