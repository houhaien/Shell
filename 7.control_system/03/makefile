CC := gcc
CFLAGS := -Wall -g -lpthread
OBJS = master.o

master:$(OBJS)
	$(CC) -o master $(CFLAGS) $(OBJS)

master.o: master.h master.c
	$(CC) -c $(CFLAGS) -o master.o master.c

.PHONY: clean

clean:
	rm -f *.o master
	echo Done Cleaning !

install: 
	mkdir /opt/master
	mkdir /opt/master/log/
	mkdir /opt/master/Worklog/
	cp pihealthd.conf /opt/master/pihealthd.conf
	cp -f master /usr/bin/
	echo Install OK !


uninstall:
	rm -r /opt/master/


