if [ -e ./setup.lock ]; then
	echo info: lab4-factory already setup
	exit 0
fi

echo info: setup lab4-factory environment

failed=0

if [ ! -e ./src/hex2raw.c ]; then
    echo warn: missing hex2raw.c
    failed=$[$failed + 1]
else
    gcc -m32 -o ./hex2raw ./src/hex2raw.c -w
    echo -n "info: building hex2raw"
    for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
    echo " done"
fi

if [ ! -e ./src/raw2hex.c ]; then
    echo warn: missing raw2hex.c
    failed=$[$failed + 1]
else
    gcc -m32 -o ./raw2hex ./src/raw2hex.c -w
    echo -n "info: building raw2hex"
    for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
    echo " done"
fi

if [ ! -e ./src/read-course.c ]; then
    echo warn: missing read-course.c
    failed=$[$failed + 1]
else
    gcc -m32 -o ./read-course ./src/read-course.c -w
    echo -n "info: building read-course"
    for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
    echo " done"
fi


if [ ! -e ./src/gen-main.c ]; then
	echo warn: missing gen-main.c
	failed=$[$failed + 1]
else
	gcc -m32 -o ./gen-main ./src/gen-main.c -w
	echo -n "info: building gen-main"
	for ((i = 0; i < 3; ++i)); do echo -n "."; sleep 0.1; done
	echo " done"
fi

for ((i = 1; i <= 5; ++i)); do
	if [ -e ./src/solve-phase$i.c ]; then
		echo -n "info: build solve-phase$i"
		for ((j = 0; j < 3; ++j)); do echo -n "."; sleep 0.1; done
		gcc -m32 -o ./solve-phase$i ./src/solve-phase$i.c
		echo " done"
	else
		echo warn: missing solve-phase$i.c
		failed=$[$failed + 1]
	fi
done

if [ $failed -eq 0 ]; then
	echo > ./setup.lock
	echo info: environment setup successfully
else
	echo warn: failed $failed targets when setup environment
	exit 1
fi
