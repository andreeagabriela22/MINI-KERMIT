#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

char SOH;
char LEN;
char SEQ = 100;
char TYPE;
char MAXL;
char TIME;
char NPAD;
char PADC;
char EOL;
char QCTL;
char QBIN;
char CHKT;
char REPT;
char CAPA;
char R;
char MARK;

char currentSEQ;

msg Y_message() {
	char TYPE = 'Y';
	short int CHECK = 0;


	msg m;
	m.len = 7;
	m.payload[0] = SOH;
	m.payload[1] = LEN;
	m.payload[2] = currentSEQ + 1;
	m.payload[3] = TYPE;
	m.payload[4] = (char) CHECK;
	m.payload[5] = (char) (CHECK >> 8);
	m.payload[6] = MARK;

	return m;
}

msg N_message() {
	char TYPE = 'N';
	short int CHECK = 0;	
	
	msg m;
	m.len = 7;
	m.payload[0] = SOH;
	m.payload[1] = LEN;
	m.payload[2] = currentSEQ + 1;
	m.payload[3] = TYPE;
	m.payload[4] = (char) CHECK;
	m.payload[5] = (char) (CHECK >> 8);
	m.payload[6] = MARK;

	return m;
}

msg receiveMessageForSure() {
	msg r;

	while (1) {
		recv_message(&r);

		LEN = r.payload[1];
		

		char CHECK1, CHECK2;
		CHECK2 = r.payload[LEN];
		CHECK1 = r.payload[LEN-1];
		r.payload[LEN-1] = 0;
		r.payload[LEN] = 0;	

		short int newCHECK = crc16_ccitt(r.payload, r.len);

		if ((char)newCHECK == CHECK1 && (char)(newCHECK >> 8) == CHECK2) {
			currentSEQ = r.payload[2];			
			msg y = Y_message();
			send_message(&y);

			if (r.payload[2] == SEQ)
				continue;
			else {
				SEQ = r.payload[2];
				return r;
			}
		} else {
			msg n = N_message();
			send_message(&n);
		}
	}
}


void recvInit() {
	msg r = receiveMessageForSure();

	SOH = r.payload[0];
	LEN = r.payload[1];
	SEQ = r.payload[2];
	TYPE = r.payload[3];
	MAXL = r.payload[4];
	TIME = r.payload[5];
	NPAD = r.payload[6];
	PADC = r.payload[7];
	EOL = r.payload[8];
	QCTL = r.payload[9];
	QBIN = r.payload[10];
	CHKT = r.payload[11];
	REPT = r.payload[12];
	CAPA = r.payload[13];
	R = r.payload[14];
	MARK = r.payload[17];
}

char* recvFileHeader(msg r) {
	
	SOH = r.payload[0];
	LEN = r.payload[1];
	SEQ = r.payload[2];
	TYPE = r.payload[3];

	char* DATA = malloc(LEN - 5);
	int j = 0;
	
	for (int i = 4; i < LEN-1; i++) {
		DATA[j] = r.payload[i];
		j++;
	}
	MARK = r.payload[LEN+1];

	return DATA;	
}

void recvFile(msg r) {
	char* FileHeader = recvFileHeader(r);
	char FileName[250] = "recv_";

	strcat(FileName, FileHeader);

	FILE* f = fopen(FileName, "wb");
	msg t;

	while (1) {
		t = receiveMessageForSure();
	
		SOH = t.payload[0];
		LEN = t.payload[1];
		SEQ = t.payload[2];
		TYPE = t.payload[3];

		MARK = t.payload[LEN+1];

		if (recvEOF(t))
			break;

		fwrite(t.payload + 4, 1, LEN - 5, f);
	}

	fclose(f);
}

int recvEOF(msg r) {
	if (r.payload[3] == 'Z') {
		
		return 1;
	}
	else
		return 0;
}

int main(int argc, char** argv) {
	msg r, t;

	init(HOST, PORT);

	recvInit();

	while (1) {
		r = receiveMessageForSure();

		if (r.payload[3] == 'B')
			break;
		
		recvFile(r);
	}
	
	return 0;
}
