#!/bin/bash

i=1
MAX=100

while [ $i -le $MAX ]
do
	./client -i 127.0.0.1 -p 8787 -s 10 &
	i=`expr $i + 1`
done
