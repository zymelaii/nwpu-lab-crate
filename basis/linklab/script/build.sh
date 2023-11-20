if [ -z "$1" ]; then
	echo error: require student id
	exit
fi

if [ ! -e $1 ]; then
	echo error: missing target ./$1
fi

fnerr=$1/stderr.log

if [ -e $1/phase1.o ]; then
	if [ -e solve-phase1 ]; then
		echo -n "info: building the solution to phase1"
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		gcc -m32 -o $1/phase1.pre $1/main.o $1/phase1.o
		./solve-phase1 $1 "`./$1/phase1.pre`" < $1/phase1.o > $1/phase1.bin 2> /dev/null
		rm $1/phase1.pre
	else
		echo -n "warn: missing solution to phase1, building hack version"
		./script/gen-hack.sh $1 1 > /dev/null
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		gcc -m32 -c -o $1/phase1.bin $1/phase1-hack.c -w
		rm $1/phase1-hack.c
	fi
	gcc -m32 -o $1/linkbomb1 $1/main.o $1/phase1.bin 2>> $fnerr
	if [ $? -eq 0 ]; then echo " done"; else echo " failed"; fi
fi

if [ -e $1/phase2.o ]; then
	if [ -e solve-phase2 ]; then
		echo -n "info: building the solution to phase2"
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		./solve-phase2 < $1/phase2.o > $1/phase2.bin 2> /dev/null
	else
		echo -n "warn: missing solution to phase2, building hack version"
		./script/gen-hack.sh $1 2 > /dev/null
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		gcc -m32 -c -o $1/phase2.bin $1/phase2-hack.c -w
		rm $1/phase2-hack.c
	fi
	gcc -m32 -o $1/linkbomb2 $1/main.o $1/phase2.bin 2>> $fnerr
	if [ $? -eq 0 ]; then echo " done"; else echo " failed"; fi
fi

if [ -e $1/phase3.o ]; then
	if [ -e solve-phase3 ]; then
		echo -n "info: building the solution to phase3"
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		./solve-phase3 $1 < $1/phase3.o > $1/phase3_patch.c 2> /dev/null
		gcc -m32 -c -o $1/phase3_patch.o $1/phase3_patch.c
		gcc -m32 -o $1/linkbomb3 $1/main.o $1/phase3.o $1/phase3_patch.o 2>> $fnerr
	else
		echo -n "warn: missing solution to phase3, building hack version"
		./script/gen-hack.sh $1 3 > /dev/null
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		gcc -m32 -c -o $1/phase3.bin $1/phase3-hack.c -w
		rm $1/phase3-hack.c
		gcc -m32 -o $1/linkbomb3 $1/main.o $1/phase3.bin 2>> $fnerr
	fi
	if [ $? -eq 0 ]; then echo " done"; else echo " failed"; fi
fi

if [ -e $1/phase4.o ]; then
	if [ -e solve-phase4 ]; then
		echo -n "info: building the solution to phase4"
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		./solve-phase4 $1 < $1/phase4.o > $1/phase4.bin 2> /dev/null
	else
		echo -n "warn: missing solution to phase4, building hack version"
		./script/gen-hack.sh $1 4 > /dev/null
		for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
		gcc -m32 -c -o $1/phase4.bin $1/phase4-hack.c -w
		rm $1/phase4-hack.c
	fi
	gcc -m32 -o $1/linkbomb4 $1/main.o $1/phase4.bin 2>> $fnerr
	if [ $? -eq 0 ]; then echo " done"; else echo " failed"; fi
fi

if [ -e $1/phase5.o ]; then
	echo -n "info: building the solution to phase5"
	for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
	./solve-phase5 < $1/phase5.o > $1/phase5.bin 2> /dev/null
	gcc -m32 -o $1/linkbomb5 $1/main.o $1/phase5.bin 2>> $fnerr
	if [ $? -eq 0 ]; then echo " done"; else echo " failed"; fi
fi
