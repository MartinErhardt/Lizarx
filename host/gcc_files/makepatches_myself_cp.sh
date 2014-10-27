GCC_PATH_TO=../srcs/gcc-4.9.1
mkdir -p ./libgcc
mkdir -p ./libstdc++-v3
mkdir -p ./gcc
cp $GCC_PATH_TO/libgcc/config.host		./libgcc/config.host
cp $GCC_PATH_TO/libstdc++-v3/crossconfig.m4	./libstdc++-v3/crossconfig.m4
cp $GCC_PATH_TO/gcc/config.gcc			./gcc/config.gcc
cp $GCC_PATH_TO/config.sub			./config.sub

cp $GCC_PATH_TO/libgcc/config.host		./libgcc/config.orig.host
cp $GCC_PATH_TO/libstdc++-v3/crossconfig.m4	./libstdc++-v3/crossconfig.orig.m4
cp $GCC_PATH_TO/gcc/config.gcc			./gcc/config.orig.gcc
cp $GCC_PATH_TO/config.sub			./config.orig.sub
