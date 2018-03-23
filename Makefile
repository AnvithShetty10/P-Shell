includepath = ./include/
srcpath = ./src/
files = $(srcpath)psh.c $(srcpath)pista_main.c $(srcpath)cmd_processor.c $(srcpath)dir_stuff.c

all: drun

run: compile
	./psh

compile:
	cc -I$(includepath) -Wall -o psh $(files)

drun: dcompile
	./psh

dcompile:
	cc -I$(includepath) -DDEBUG_MODE -Wall -o psh $(files)
