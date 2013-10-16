################################################################################
 #
 # Project name:        IFJ - Team project
 # Filename:            errors.h
 # Author:              Marcel Fiala
 # Encoding:            UTF-8
 #
 # Description:         Definition of error codes.
 #
################################################################################

NAME=ifj_070
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic
ARCHIVE_NAME=xvecer17

OBJFILES=main.o

VGPARAMS=--tool=memcheck --leak-check=yes -v --show-reachable=yes \
		 --track-origins=yes --trace-children=yes --show-possibly-lost=yes

.PHONY: link debug dep clean clndep pack run

# creation of object files
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# start rule
all: dep link clean

# linking object files in one executable file
link: $(NAME)

# compilation for debugging
debug: CFLAGS+=-g3
debug: dep link clndep

# creating dependencies list
dep:
	$(CC) -MM *.c > dependencies.list

# include of created dependencies
-include dependencies.list

# linking object files in one executable file
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

#run
run:
	./$(NAME)

# valgrind test
grind:
	valgrind $(VGPARAMS) ./$(NAME)

# cleaning unnecessary files
clean:
	-rm -r dependencies.list $(OBJFILES)

# deleting dependencies list
clndep:
	-rm dependencies.list

# pack
pack:
	zip -r $(ARCHIVE_NAME).zip *.c *.h Makefile

### End of file Makefile ###
