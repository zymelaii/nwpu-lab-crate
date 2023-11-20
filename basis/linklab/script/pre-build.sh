if [ -z "$1" ]; then
	echo error: require student id
	exit
fi

if [ ! -e $1 ]; then
	echo error: missing target ./$1
fi

for i in `ls $1/phase*.hex 2> /dev/null`; do
	phase=${i:11:6}.o
	if [ ! -e $1/$phase ]; then
		./hex2raw < $i > $1/$phase
		echo info: resolve $phase by $i
	else
		echo info: ignore $i
	fi
done
