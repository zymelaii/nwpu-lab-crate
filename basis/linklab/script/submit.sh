if [ -z "$1" ]; then
	echo error: require student id
	exit
fi

if [ ! -e $1 ]; then
	echo error: missing target ./$1
	exit
fi

cd ./$1
echo info: start submission task of student $1
for ((i = 1; i <= 5; ++i)); do
	echo "[|RUN phase $i|]"
	if [ -e ./linkbomb$i ]; then
		./linkbomb$i -s -p "`cat password`" 1> /dev/null
		if [ $? -eq 0 ]; then
			echo "[[       pass]]"
		elif [ $? -eq 1 ] && [ $i -eq 2 ]; then
                        echo "[[       pass]]"
		else
			echo "[[     failed]]"
		fi
		sleep 0.1
	else
		echo "[[       skip]]"
	fi
done
echo info: submit complete
