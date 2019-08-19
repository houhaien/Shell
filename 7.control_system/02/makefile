CC := gcc
CFLAGS := -Wall -g -lpthread
OBJS = client.o 

client: client.o
	$(CC) -o client $(CFLAGS) $(OBJS)

client.o : client.h 5.client.c 
	$(CC) -c $(CFLAGS) -o client.o 5.client.c

.PHONY: clean



clean:
	rm -f *.o client
	echo Done cleaning !

install:
	mkdir /opt/client
	mkdir /opt/client/log
	mkdir /opt/client/script
	cp pihealthc.conf /opt/client/pihealthc.conf
	cp ./cpu_info.sh /opt/client/script
	cp ./Control_Disk.sh /opt/client/script
	cp ./Error_course.sh /opt/client/script
	cp ./Memlog.sh /opt/client/script
	cp ./Users.sh /opt/client/script
	cp ./SysInfo.sh /opt/client/script
	cp -f client /usr/bin/
	echo Install OK !

uninstall:
	rm -r /opt/client/

	


