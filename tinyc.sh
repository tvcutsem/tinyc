#!/bin/sh
#
#	Usage:	tinyc basename
#
#	e.g. "tinyc tst"  to compile tst.tiny, 
#
#	First make sure source file exists
#
if [ ! -f "$1.tiny" ] 
then
	echo "cannot open \"$1.tiny\""
	exit 1
fi
#
if ./tiny $1 < $1.tiny > $1.jasm
then
	javaa < $1.jasm
fi
