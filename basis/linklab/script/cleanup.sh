if [ ! -z "$1" ] && [ -e $1 ] && [ -d $1 ]; then
	rm -rf $1
	echo info: target $1 cleaned-up
else
	echo info: sweeping environment
	rm -rf hex2raw raw2hex read-course gen-main solve-phase* setup.lock *.log
	echo info: cleanup done
fi
