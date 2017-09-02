#ifndef LIB
#define LIB

typedef struct {
    int len;
    char payload[1400];
} msg;

void init(char* remote, int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);
msg* receive_message_timeout(int timeout); //timeout in milliseconds
unsigned short crc16_ccitt(const void *buf, int len);

void msgDump(char* antet, msg* m) {
	printf("%s : %c : ", antet, m->payload[3]);
	for (int i = 0; i < m->len; i++) {
		printf("%d ", m->payload[i]);
	}
	printf("\n");
}
#endif

