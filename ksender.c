#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

char secventa = 0;
char MAXL = 250;
char TIME = 5;

char* sendInit() {
	char SOH = 1;
	char LEN = 16;
	char SEQ = 0;
	char TYPE = 'S';
	char NPAD = 0;
	char PADC = 0;
	char EOL = '\r';
	char QCTL = 0;
	char QBIN = 0;
	char CHKT = 0;
	char REPT = 0;
	char CAPA = 0;
	char R = 0;
	short int CHECK = 0;
	char MARK = '\r';

	char* data = malloc(18);

	data[0] = SOH;
	data[1] = LEN;
	data[2] = SEQ;
	data[3] = TYPE;
	data[4] = MAXL;
	data[5] = TIME;
	data[6] = NPAD;
	data[7] = PADC;
	data[8] = EOL;
	data[9] = QCTL;
	data[10] = QBIN;
	data[11] = CHKT;
	data[12] = REPT;
	data[13] = CAPA;
	data[14] = R;
	data[15] = (char) CHECK;
	data[16] = (char) (CHECK >> 8);
	data[17] = MARK;
	
	CHECK = crc16_ccitt(data, 18);

	data[15] = (char) CHECK;
	data[16] = (char) (CHECK >> 8);

	return data;
}

char* sendFileHeader(char* fileName) {
	char SOH = 1;
	char LEN = 5 + strlen(fileName);
	char SEQ = secventa;
	char TYPE = 'F';
	char* DATA = fileName;
	short int CHECK = 0;
	char MARK = '\r';

	char* data = malloc(LEN + 2);

	data[0] = SOH;
	data[1] = LEN;
	data[2] = SEQ;
	data[3] = TYPE;

	int j = 0;

	for (int i = 4; i < LEN-1; i++) {
		data[i] = DATA[j];
		j++;
	}

	data[LEN-1] = (char) CHECK;
	data[LEN] = (char) (CHECK >> 8);
	data[LEN+1] = MARK;

	CHECK = crc16_ccitt(data, LEN + 2);

	data[LEN-1] = (char) CHECK;
	data[LEN] = (char) (CHECK >> 8);

	return data;
}

void sendFile(char* FileName) {
	FILE* f = fopen(FileName, "rb");
	char buf[MAXL];

	while (1) {
		int size = fread(buf, 1, MAXL, f);
		if (size <= 0) 
			break;

		msg t;
		t.len = size + 7;
		t.payload[0] = 1;
		t.payload[1] = size + 5;
		t.payload[2] = secventa;
		t.payload[3] = 'D';

		memcpy(t.payload + 4, buf, size);

		t.payload[t.len - 1] = '\r';
		t.payload[t.len - 2] = 0;
		t.payload[t.len - 3] = 0;
		
		short int CHECK = crc16_ccitt(t.payload, t.len);
		
		t.payload[t.len - 3] = (char) CHECK;
		t.payload[t.len - 2] = (char) (CHECK >> 8);

		sendMessageForSure(t);
	}

	fclose(f);
	
	sendEOF();
}

void sendEOF() {
	msg t;
	t.len = 7;
	t.payload[0] = 1;
	t.payload[1] = 5;
	t.payload[2] = secventa;
	t.payload[3] = 'Z';
	t.payload[4] = 0;
	t.payload[5] = 0;
	t.payload[6] = '\r';

	short int CHECK = crc16_ccitt(t.payload, t.len);

			
	t.payload[4] = (char) CHECK;
	t.payload[5] = (char) (CHECK >> 8);

	sendMessageForSure(t);
}

void sendEOT() {
	msg t;
	t.len = 7;
	t.payload[0] = 1;
	t.payload[1] = 5;
	t.payload[2] = secventa;
	t.payload[3] = 'B';
	t.payload[4] = 0;
	t.payload[5] = 0;
	t.payload[6] = '\r';

	short int CHECK = crc16_ccitt(t.payload, t.len);

			
	t.payload[4] = (char) CHECK;
	t.payload[5] = (char) (CHECK >> 8);

	sendMessageForSure(t);
	
}

void sendMessageForSure(msg t) {
	int succes = 0;

	while (succes == 0) {
		send_message(&t);

		msg *y = receive_message_timeout(5000);
		
		if (y == NULL) {
		} else if (y->payload[3] == 'N') {
			secventa += 2;
		} else if (y->payload[3] == 'Y') {
			succes = 1;
			secventa += 2;
		} 
				
	}
}

int main(int argc, char** argv) {
	msg t;
	init(HOST, PORT);

	memcpy(t.payload, sendInit(), 18);
	t.len = 18;
	sendMessageForSure(t);

	for (int i = 1; i < argc; i++) {
		char* FileHeader = sendFileHeader(argv[i]);
		memcpy(t.payload, FileHeader, 2 + FileHeader[1]);
		t.len = 2 + FileHeader[1];
		sendMessageForSure(t);
		sendFile(argv[i]);
	}

	sendEOT();
        
	return 0;
}
