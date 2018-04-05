includepath = ./include/
srcpath = ./src/
files = $(srcpath)psh.c $(srcpath)pista_main.c $(srcpath)cmd_processor.c

all: drun

run: compile
	./psh

compile:
	cc -I$(includepath) -Wall -o psh $(files)

drun: dcompile
	./psh

dcompile:
	cc -v -I$(includepath) -DDEBUG_MODE -g -Wall -o psh $(files)
