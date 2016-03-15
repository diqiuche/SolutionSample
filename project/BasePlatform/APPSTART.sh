#!/bin/bash

# Start engine script

start_engine()
{
	export LD_LIBRARY_PATH=~/appsample/lib:$LD_LIBRARY_PATH
	export CONFIG_FILE_PATH=~/appsample/etc/db.conf
	export COUT_LOG_LEVEL=1
	export FILE_LOG_LEVEL=1
	export ENABLE_COUT_LOG=1
	export ENABLE_FILE_LOG=1
	cd ~/appsample/bin
	~/appsample/bin/exe_BasePlatform
}

start_engine

