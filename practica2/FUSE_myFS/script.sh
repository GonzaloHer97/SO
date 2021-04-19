#!/bin/bash

MOUNT_POINT="./mount-point"
TEMP="./temp"

FILE_COPY_ONE="fuseLib.c"
FILE_COPY_TWO="myFS.h"
FILE_COPY_THREE="fuseLib.h"

rm -R -f temp
mkdir temp
#a)
cp ./src/$FILE_COPY_ONE $MOUNT_POINT/
cp ./src/$FILE_COPY_TWO $MOUNT_POINT/

cp ./src/$FILE_COPY_ONE $TEMP/
cp ./src/$FILE_COPY_TWO $TEMP/
#b)
./my-fsck-static-64 virtual-disk 

diff ./src/$FILE_COPY_ONE $MOUNT_POINT/$FILE_COPY_ONE
diff ./src/$FILE_COPY_TWO $MOUNT_POINT/$FILE_COPY_TWO

truncate -o -s -1 $TEMP/$FILE_COPY_ONE
truncate -o -s -1 $MOUNT_POINT/$FILE_COPY_ONE

#c)
./my-fsck-static-64 virtual-disk

diff ./src/$FILE_COPY_ONE $MOUNT_POINT/$FILE_COPY_ONE

#d)
cp ./src/$FILE_COPY_THREE $MOUNT_POINT/

#e)
./my-fsck-static-64 virtual-disk

diff ./src/$FILE_COPY_THREE $MOUNT_POINT/$FILE_COPY_THREE

#f)
truncate -o -s +1 $TEMP/$FILE_COPY_TWO
truncate -o -s +1 $MOUNT_POINT/$FILE_COPY_TWO

#g)
./my-fsck-static-64 virtual-disk
diff ./src/$FILE_COPY_TWO $MOUNT_POINT/$FILE_COPY_TWO