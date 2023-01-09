#
#
#	Makefile for test programs
#
#

all: simulador monitor

simulador: util.o simulador.o simulador_config.o
	gcc -lpthread -lrt -g -o simulador util.o simulador.o simulador_config.o

monitor: util.o monitor.o monitor_config.o 
	gcc -g -o monitor util.o monitor.o monitor_config.o

clean:
	rm -f *.o *~ *.log simulador monitor

