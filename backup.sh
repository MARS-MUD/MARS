#!/bin/sh
#
# MUD Backup script by Dan aka Darkwolf aka Mnementh
# mudadmin@daisy.goodnet.com
# modified by Om for MARS
# ro@wr3tch.com

#
# Set the backup dir. The Path you want the backup files to reside
#
BDIR=./backup/

#
# Set the dir that MUD resides in
#
CDIR=../MARS/

#
# These are the tar flags. Man tar for info on the flags,
# Default shows verbose output of whats happinin, take v out
# if you dont like this
#
TFLAGS=cvf

#
# The gzip lvl, or level of compression
# -1 is lowest and fastest, -9 is best and slowest
#
GZL='-9'

#
# Edit this only if tar, mv, or gzip is somewhere wierd :)
#
PATH=/bin:/usr/bin

##################################################################
# End of user spec's.                                            #
# Do not edit below this line unless you know what you are doing #
##################################################################


DATE=`date +%m.%d-%H:%M`
cd $CDIR

case "$1" in
   all)
	tar $TFLAGS $BDIR/player_info.$DATE.tar gods player data
	gzip $GZL $BDIR/player_info.$DATE.tar
	echo Done with $BDIR/player_info.$DATE.tar.gz

	tar $TFLAGS $BDIR/area.$DATE.tar area doc
	gzip $GZL $BDIR/area.$DATE.tar
	echo Done with $BDIR/area.$DATE.tar.gz

        tar $TFLAGS $BDIR/log.$DATE.tar log 
	gzip $GZL $BDIR/log.$DATE.tar
	echo Done with $BDIR/log.$DATE.tar.gz

	tar $TFLAGS $BDIR/data.$DATE.tar data 
	gzip $GZL $BDIR/data.$DATE.tar
	echo Done with $BDIR/data.$DATE.tar.gz

	tar $TFLAGS $BDIR/export.$DATE.tar export 
	gzip $GZL $BDIR/export.$DATE.tar
	echo Done with $BDIR/export.$DATE.tar.gz

	tar $TFLAGS $BDIR/boards.$DATE.tar boards 
	gzip $GZL $BDIR/boards.$DATE.tar
	echo Done with $BDIR/boards.$DATE.tar.gz

	tar $TFLAGS $BDIR/uploads.$DATE.tar uploads 
	gzip $GZL $BDIR/uploads.$DATE.tar
	echo Done with $BDIR/uploads.$DATE.tar.gz
        
	tar $TFLAGS $BDIR/system.$DATE.tar system 
	gzip $GZL $BDIR/system.$DATE.tar
	echo Done with $BDIR/system.$DATE.tar.gz

	tar $TFLAGS $BDIR/projects.$DATE.tar projects 
	gzip $GZL $BDIR/projects.$DATE.tar
	echo Done with $BDIR/projects.$DATE.tar.gz

	tar $TFLAGS $BDIR/src.$DATE.tar src 
	gzip $GZL $BDIR/src.$DATE.tar
	echo Done with $BDIR/src.$DATE.tar.gz
	

	tar $TFLAGS $BDIR/all.$DATE.tar area boards data doc gods log player projects src system uploads
	gzip $GZL $BDIR/all.$DATE.tar
	echo Done with $BDIR/all.$DATE.tar.gz
	;;
	
   player)
	tar $TFLAGS $BDIR/player_info.$DATE.tar gods player data
	gzip $GZL $BDIR/player_info.$DATE.tar
	echo Done with $BDIR/player_info.$DATE.tar.gz
	;;
   area)
        tar $TFLAGS $BDIR/area.$DATE.tar area doc
        gzip $GZL $BDIR/area.$DATE.tar
        echo Done with $BDIR/area.$DATE.tar.gz
	;;
    log)
        tar $TFLAGS $BDIR/log.$DATE.tar log
        gzip $GZL $BDIR/log.$DATE.tar
        echo Done with $BDIR/log.$DATE.tar.gz
	;;
    data)
        tar $TFLAGS $BDIR/data.$DATE.tar data
        gzip $GZL $BDIR/data.$DATE.tar
        echo Done with $BDIR/data.$DATE.tar.gz
	;;
   export)
        tar $TFLAGS $BDIR/export.$DATE.tar export
        gzip $GZL $BDIR/export.$DATE.tar
        echo Done with $BDIR/export.$DATE.tar.gz
	;; 
   boards)
        tar $TFLAGS $BDIR/boards.$DATE.tar boards
        gzip $GZL $BDIR/boards.$DATE.tar
        echo Done with $BDIR/boards.$DATE.tar.gz
	;;
   uploads)
        tar $TFLAGS $BDIR/uploads.$DATE.tar uploads
        gzip $GZL $BDIR/uploads.$DATE.tar
        echo Done with $BDIR/uploads.$DATE.tar.gz
	;;
   system)
        tar $TFLAGS $BDIR/system.$DATE.tar system
        gzip $GZL $BDIR/system.$DATE.tar
        echo Done with $BDIR/system.$DATE.tar.gz
	;;

   projects)
        tar $TFLAGS $BDIR/projects.$DATE.tar projects 
        gzip $GZL $BDIR/projects.$DATE.tar
        echo Done with $BDIR/projects.$DATE.tar.gz
	;;
	
   src)
        tar $TFLAGS $BDIR/src.$DATE.tar src 
        gzip $GZL $BDIR/src.$DATE.tar
        echo Done with $BDIR/src.$DATE.tar.gz
	;;
   *)
	echo "Usage Backup {all|player|data|export|log|boards|system|uploads|area|src}"
	exit 1
esac

exit 0
