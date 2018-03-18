includepath = ./include/
srcpath = ./src/
files = $(srcpath)psh.c $(srcpath)cmd_helper.c $(srcpath)cmd_processor.c $(srcpath)dir_stuff.c

all: run

run: compile
	./psh

compile:
	cc -I$(includepath) -Wall -o psh $(files)

