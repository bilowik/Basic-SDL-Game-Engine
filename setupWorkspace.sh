#!/bin/bash

main () {
	SRC_FILES="Makefile build-sprite-db.c"
	CURR_DIR=src/
	CONTENTS_OF_CURR_DIR=$CURR_DIR/*
	r_findsrcfiles src
	vim $SRC_FILES
} # end main



r_findsrcfiles () {
	local filesInDir=$1/*
	for file in $filesInDir; do
		if [ -d $file ]; then
			r_findsrcfiles $file
		else
			SRC_FILES="$SRC_FILES $file"
		fi
	done
} # end r_findsrcFiles




main "$@"





