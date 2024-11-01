CC = gcc

.DEFAULT: all

all: restapid

restapid: restapid.c
	#sudo systemctl stop restapid.service
	$(CC) restapid.c -o restapid -lulfius
	#sudo systemctl restart restapid.service

clean: restapid
	rm restapid
