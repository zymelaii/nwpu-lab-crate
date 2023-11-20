if [ -z "$1" ]; then
	echo error: require student id
	exit
fi

if [ $2 -ge 1 ] && [ $2 -le 4 ]; then
	:
else
	echo error: hack code is only available to phase1~4
	exit
fi

E1="extern int notify\;"
E2="void\(\) { if \(notify\) client\($2, \"$1\"\)\; \}"
E3="void \(*phase\)\(\) = hack\;"

fnphase=$1/phase$2-hack.c

echo > $fnphase
echo "extern int notify;                 " >> $fnphase
echo "void hack() {                      " >> $fnphase
echo "    puts(\"$1\");                  " >> $fnphase
echo "    if (notify) client($2, \"$1\");" >> $fnphase
echo "}                                  " >> $fnphase
echo "void (*phase)() = hack;            " >> $fnphase

echo info: generate phase$2 hack code
