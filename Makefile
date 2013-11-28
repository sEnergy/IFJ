################################################################################
 #
 # Project name:		IFJ - Team project
 # Filename:			errors.h
 # Author:			  Marcel Fiala
 # Encoding:			UTF-8
 #
 # Description:		 Definition of error codes.
 #
################################################################################

NAME=ifj_070
CC=colorgcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g3
ARCHIVE_NAME=xvecer17

OBJFILES=main.o syntax_analyzer.o lex_analyzer.o ial.o token_list.o \
		 functions.o interpreter.o

VGPARAMS=--tool=memcheck --leak-check=yes -v --show-reachable=yes \
		 --track-origins=yes --trace-children=yes --show-possibly-lost=yes

.PHONY: link debug dep clean clndep pack run

# creation of object files
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# start rule
all: dep link clndep

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
	$(CC) $(CFLAGS) $(OBJFILES) -lm -o $@

#run
run:
	./$(NAME) InputFile

# valgrind test
grind:
	valgrind $(VGPARAMS) ./$(NAME) InputFile

# cleaning unnecessary files
clean:
	-rm -r dependencies.list $(OBJFILES) $(NAME)

# deleting dependencies list
clndep:
	-rm dependencies.list

# pack
pack:
	zip -r $(ARCHIVE_NAME).zip *.c *.h Makefile

### End of file Makefile ###
