echo "found in following C source files : "
OBJS=$(find . -path ./host -prune -o -name '*.c' -print)
grep -n $1 $OBJS

echo "found in following C header files : "
OBJS=$(find . -path ./host -prune -o -name '*.h' -print)
grep -n $1 $OBJS 
