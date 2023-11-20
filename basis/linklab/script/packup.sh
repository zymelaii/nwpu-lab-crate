if [ -z "$1" ]; then
	echo error: require student id
	exit
fi

if [ ! -e $1 ]; then
	echo error: missing target ./$1
	exit
fi

target=linklab-$1_lab4factory.tar
rm -rf ./$1/$target

rm -rf ./$1-packup
mkdir -p ./$1-packup/storage
cp -rf ./$1/* ./$1-packup/storage/

root="./$1-packup/storage"
lf="ls $root"

for i in `$lf | grep linkbomb`; do cp $root/$i ./$1-packup/; done
for i in `$lf | grep phase.*bin`; do cp $root/$i ./$1-packup/${i:0:7}o; done
if [ -e $root/phase3_patch.c ]; then
	cp $root/phase3_patch.c ./$1-packup/
	cp $root/phase3_patch.o ./$1-packup/
	cp $root/phase3.o ./$1-packup/
fi

cd $1-packup && tar -cf ../$1/$target * && cd ..
rm -rf ./$1-packup
