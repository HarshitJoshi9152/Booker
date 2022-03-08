# better https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# https://www.youtube.com/watch?v=Jkmy2YLUbUY
# https://github.com/tsoding/sv/blob/master/sv.h
CC=gcc
CFLAGS=-Wall -g
build_dir=../build
DEPS=html.h

_OBJ=html.o main.o bookmark.o
OBJ= $(patsubst %, $(build_dir)/%, $(_OBJ));


booker: $(OBJ)
	$(CC) $(CFLAGS) -o $(build_dir)/booker $^

$(build_dir)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

run:
	$(build_dir)/booker