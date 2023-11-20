if [ -z "$1" ]; then
	echo error: require student id
	exit
fi

if [ -z "$2" ]; then
	echo error: require course id
	exit
fi

if [ -z "$3" ]; then
	echo error: require password
	exit
fi

./script/setup.sh
sleep 0.2

./script/init.sh $1 $2 $3
sleep 0.2

./script/pre-build.sh $1
sleep 0.2

./script/build.sh $1
sleep 0.2

./script/submit.sh $1
sleep 0.2

./script/packup.sh $1
