echo "delete the following .o files: "
OBJS=$(find . -path ./host -prune -o -name '*.o' -print)
echo $OBJS
rm $OBJS
