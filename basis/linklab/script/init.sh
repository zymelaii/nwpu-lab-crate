if [ ! -e setup.lock ]; then
	if [ -e ./script/setup.sh ]; then
		./script/setup.sh
	else
		echo error: missing setup.sh with environment unset
		exit 1
	fi
	sleep 0.1
	if [ ! $? -eq 0 ]; then exit 1; fi
fi

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

if [ -e $1 ] && [ -d $1 ]; then
	echo warn: target $1 already exists
	exit
fi

mkdir $1
echo $3 > $1/password
./gen-main $1 $2 > $1/main.o

root="./assets/U10M21003"
fnlab="$root/$2/linklab-$1.tar"
phases="phase1.o phase2.o phase3.o phase4.o phase5.o"
if [ -e $fnlab ]; then
	tar -xf $fnlab -C ./$1  $phases
fi
