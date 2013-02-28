#!/bin/sh
#
# a script to build nsock_test.c from the current directory listing
#

# first, make a list of .c files to use, without their extension
echo -n "getting files..."
FILEZ=""
for ii in *.c; do
   FNAME=`echo $ii | rev | cut -c 3- | rev`
   if [ "$FNAME" != "nsock_test" ]; then
      if [ -z "$FILEZ" ]; then
        FILEZ="$FNAME"
      else
        FILEZ="$FILEZ $FNAME"
      fi
   fi
done
echo ".done"

# ok build the file.. start with prototypes..
echo -n "building nsock_test.c..."
echo > nsock_test.c
for ii in $FILEZ; do
   echo "void "$ii"(void);" >> nsock_test.c
done

# some whitespace and then main..
echo "" >> nsock_test.c
echo "int" >> nsock_test.c
echo "main(void)" >> nsock_test.c
echo "{" >> nsock_test.c

# the function calls
for ii in $FILEZ; do
   echo "    "${ii}"();" >> nsock_test.c
done

# the end of main, return with success
echo "    return 0;" >> nsock_test.c
echo "}" >> nsock_test.c
echo ".done"


# build the makefile include
echo -n "building srcobj.mk..."
echo > srcobj.mk
# srcs
echo "SRCS = \\" >> srcobj.mk
for ii in $FILEZ; do
   echo "	"$ii".c \\" >> srcobj.mk
done
echo "" >> srcobj.mk
# objs..
echo "OBJS = \\" >> srcobj.mk
for ii in $FILEZ; do
   echo "	"$ii".o \\" >> srcobj.mk
done
echo "" >> srcobj.mk
echo ".done"

# add dependencies
echo -n "updating dependencies..."
cat Makefile.in > Makefile
gcc -MM -Wall -I.. *.c >> Makefile
echo ".done"

exit 0
