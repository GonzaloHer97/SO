#!/bin/bash

if test -e mytar -a -x mytar; then 
	
	if test -e tmp;	then
		rm -r tmp 
	fi 
	
	mkdir tmp
	cd ./tmp

	echo "Hello world!" > file1.txt
	head -n 10 /etc/passwd > file2.txt
	head -c 1024 /dev/urandom > file3.dat
	
	./../mytar -c -f filetar.mtar file1.txt file2.txt file3.dat

	mkdir out
	mv filetar.mtar ./out/
	cd ./out
	./../../mytar -x -f filetar.mtar
	cd ..

	diff file1.txt ./out/file1.txt
	diff1=$?
	diff file2.txt ./out/file2.txt
	diff2=$?
	diff file3.dat ./out/file3.dat
	diff3=$?

	diffs=$(($diff1+$diff2+$diff3))

	if [ $diffs -gt 0 ]; then 
		exit 1
	else
		echo "Correct"
		exit 0
	fi

else
	echo "Mytar no existe o no es ejecutable"
fi