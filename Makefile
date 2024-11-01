CC = gcc

.SILENT: all restapid
.DEFAULT: all

all: restapid

restapid:
	#sudo systemctl stop restapid.service
	$(CC) restapid.c -o restapid -lulfius
	#sudo systemctl restart restapid.service

clean: restapid
	rm restapid
