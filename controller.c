#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include<pthread.h>
#include<assert.h>

#include<semaphore.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

FILE out_file;

const size_t buffsize=4096;
int fd, err;
int fd2, err2;
struct addrinfo *address;
struct addrinfo *address2;
void sendCommand(char msg[buffsize]);

pthread_t thread1, thread2, thread3;
int rc1, rc2, rc3;

void *logFile(void *arg){
	char message[buffsize];
	char incoming[buffsize];
	size_t msgsize;
	FILE *out_file = fopen("README", "a+");
	char *msg = (char*)arg;
	fprintf(out_file, "%s\n", msg);

	strcpy(message, "state:?");
	sendto(fd, message, strlen(message), 0, address->ai_addr, address->ai_addrlen);
	msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
	incoming[msgsize] = '\0';
	fprintf(out_file, "%s\n", incoming);

	strcpy(message, "condition:?");
	sendto(fd, message, strlen(message), 0, address->ai_addr, address->ai_addrlen);
	msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
	incoming[msgsize] = '\0';
	fprintf(out_file, "%s\n", incoming);

	strcpy(message, "terrain:?");
	sendto(fd, message, strlen(message), 0, address->ai_addr, address->ai_addrlen);
	msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
	incoming[msgsize] = '\0';
	fprintf(out_file, "%s\n", incoming);


	fclose(out_file);

	return 0;
}

void *input(void *arg){
	char input;
	while (1)
	{
		char outgoing[buffsize];
		scanf("%s", &input);
		switch(input)
		{
			case 'w':
				strcpy(outgoing, "command:!\nmain-engine:100");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				rc3 = pthread_create(&thread3, NULL, logFile, "Throttle 100");
				assert(rc3 == 0);
				pthread_join(thread3, NULL);
				printf("throttle 100 \n");
				break;
			case 's':
				strcpy(outgoing, "command:!\nmain-engine:0");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				rc3 = pthread_create(&thread3, NULL, logFile, "Throttle 0");
				assert(rc3 == 0);
				pthread_join(thread3, NULL);
				printf("throttle 0 \n");
				break;
			case 'a':
				strcpy(outgoing, "command:!\nrcs-roll:-0.5");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				rc3 = pthread_create(&thread3, NULL, logFile, "Spin Left");
				assert(rc3 == 0);
				pthread_join(thread3, NULL);
				printf("Spin Left \n");
				break;
			case 'd':
				strcpy(outgoing, "command:!\nrcs-roll:0.5");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				rc3 = pthread_create(&thread3, NULL, logFile, "Spin Right");
				assert(rc3 == 0);
				pthread_join(thread3, NULL);
				printf("Spin Right \n");
				break;
			case 'e':
				strcpy(outgoing, "command:!\nrcs-roll:0");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				rc3 = pthread_create(&thread3, NULL, logFile, "Stop Spin");
				assert(rc3 == 0);
				pthread_join(thread3, NULL);
				printf("Stop Spin \n");
				break;
			case 'q':
				strcpy(outgoing, "terrain:?");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				rc3 = pthread_create(&thread3, NULL, logFile, "Terrain");
				assert(rc3 == 0);
				pthread_join(thread3, NULL);
				printf("Terrain \n");
				break;
			case 'r':
				strcpy(outgoing, "state:=");
				sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
				break;
			
			default:
				printf("invalid input \n");
				break;
		}

	}
}

void *getCondition(void *arg){
	while(1){
		char msg[buffsize];
		char incoming[buffsize];
		size_t msgsize;
		strcpy(msg, "condition:?");
		sendto(fd, msg, strlen(msg), 0, address->ai_addr, address->ai_addrlen);

		msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
		incoming[msgsize] = '\0';
		sendto(fd2, incoming, strlen(incoming), 0, address2->ai_addr, address2->ai_addrlen);
		usleep(100000);
	}
}

int main(int argc, char *argv[])
{
	fclose(fopen("README", "w"));

	char *host="127.0.1.1";
	char *port="65200";

	char *port2="65250";

	const struct addrinfo hints=
	{
		.ai_family=AF_INET,
		.ai_socktype=SOCK_DGRAM,
	};

	const struct addrinfo hints2=
	{
		.ai_family=AF_INET,
		.ai_socktype=SOCK_DGRAM,
	};

	err=getaddrinfo(host, port, &hints, &address);
	if (err)
	{
		fprintf(stderr, "Error in getting Address: %s\n", gai_strerror(err));
		exit(1);
	}

	err2=getaddrinfo(host, port2, &hints2, &address2);
	if (err2)
	{
		fprintf(stderr, "Error in getting Address: %s\n", gai_strerror(err2));
		exit(1);
	}

	fd=socket(AF_INET, SOCK_DGRAM, 0);
	if (fd==-1)
	{
		fprintf(stderr, "Error making socket: %s\n", strerror(errno));
		exit(1);
	}

	fd2=socket(AF_INET, SOCK_DGRAM, 0);
	if (fd2==-1)
	{
		fprintf(stderr, "Error making socket: %s\n", strerror(errno));
		exit(1);
	}

	rc1 = pthread_create(&thread1, NULL, input, NULL);
	rc2 = pthread_create(&thread2, NULL, getCondition, NULL);

	assert(rc1 == 0);
	pthread_join(thread1,NULL);
	assert(rc2 == 0);
	pthread_join(thread2,NULL);
	assert(rc3 == 0);
	pthread_join(thread3,NULL);

}

void sendCommand(char msg[buffsize]){
	sendto(fd, msg, strlen(msg), 0, address->ai_addr, address->ai_addrlen);
}



void finished(int sig)
{
	exit(0);
}

void cleanup(void)
{
	close(fd);
}

