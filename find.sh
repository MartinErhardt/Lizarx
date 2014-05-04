echo "found in following C source files : "
OBJS=$(find . -path ./host -prune -o -name '*.c' -print)
grep $1 $OBJS

echo "found in following C header files : "
OBJS=$(find . -path ./host -prune -o -name '*.h' -print)
grep $1 $OBJS
