#!/bin/bash

for i in {1..20}; do
	./spark > "test$i.txt"
done

