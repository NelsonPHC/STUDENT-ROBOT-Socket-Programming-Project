#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define LISTEN_PORT 3310
#define ROBOT_VERSION "3.0"

int main(){


	char* robotIP;
	int ListenSocket, s_2, s2, s3;

	printf("Robot version %s (Oct. 24, 2016) started \n", ROBOT_VERSION);
	printf("You are reminded to check for the latest available version\n\n");

	// --------------------------------------------------------------------- //
	//								Step 1/2								 //
	// --------------------------------------------------------------------- //
	// Create a SOCKET for listening for incoming connection requests.
	printf("Creating TCP socket for connecting to the ROBOT...\n");
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error at creating socket...\n");
		return 1;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("0.0.0.0");
	service.sin_port = htons(LISTEN_PORT);

	int connection_status = connect(ListenSocket, (struct sockaddr *) &service, sizeof(service));
	if(connection_status == -1) printf("There was an error making connection to the remote socket\n");

	char sid[]="1155084962";

	send(ListenSocket, sid, sizeof(sid), 0); 		//change to a loop (message boundary)

	// --------------------------------------------------------------------- //
	//								Step 3									 //
	// --------------------------------------------------------------------- //
	char iTCPPort2Connect[256];
	recv(ListenSocket, &iTCPPort2Connect, sizeof(iTCPPort2Connect), 0);			//receive "ddddd"					//receive "ddddd" might need to recv() using a loop
	
	close(ListenSocket);

	int newPort = atoi(iTCPPort2Connect);		//change it to int type, newPort is now ddddd

//	Now student is the server!!
	printf("Creating a TCP socket s_2 at port %d for listening and accepting connection...", newPort);	
	s_2 = socket(AF_INET, SOCK_STREAM, 0);
	if (s_2 < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error at creating socket...\n");
		return 1;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	struct sockaddr_in service1;
	service1.sin_family = AF_INET;
	service1.sin_addr.s_addr = inet_addr("0.0.0.0");
	service1.sin_port = htons(newPort);
//



	if (bind( s_2, (struct sockaddr *) &service1, sizeof(service1)) < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("bind() failed.\n");
		close(s_2);
		return 1;
	}
	if (listen( s_2, 1 ) < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error listening on socket.\n");
		close(s_2);
		return 1;
	}
	printf("Done\n");

	printf("\nReady to accept connection on port %d\n", newPort);
	printf("Waiting for connection...\n");
	// When accept is called, the function is blocked and wait until a client is connected
	// A new socket is returned when accepting a connection

	struct sockaddr_in sin; 
	int size = sizeof(sin);
	memset(&sin, 0, size);

	s2 = accept( s_2, (struct sockaddr*)&sin, (socklen_t *)&size );
	if (s2 < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("accept failed\n");
		close(s_2);
		return 1;
	}
	//	The listen socket is no longer required
	//	Usually you can use a loop to accept new connections
	close(s_2);
	printf("\nClient from %s at port %d connected\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
	robotIP = inet_ntoa(sin.sin_addr);



//step 4
	char usocket[13];		//	“fffff,eeeee.”
	memset(usocket, 0, 13);
	int totalByteReceived = 0;
	int byteToReceive = 10;
	int iResult=0;

	//	Receive the data from the socket s2
	//	TCP socket is a stream socket which does not preserve message boundary
	//	Calling recv to receive k bytes may receive less than k bytes
	//	A loop is required
	do {
		iResult = recv(s2, usocket+totalByteReceived, 13-totalByteReceived, 0);
		if ( iResult > 0 ){
			printf("Bytes received: %d\n", iResult);
			totalByteReceived += iResult;
			if (totalByteReceived >= byteToReceive)
				break;
		}
		else if ( iResult == 0 ){
			printf(" %s %d \n",strerror(errno), errno);
			printf("Connection closed\n");
			return 1;
		}
		else{
			printf(" %s %d \n",strerror(errno), errno);			
			printf("recv failed\n");
			return 1;
		}
	} while( iResult > 0 );
	printf("Message received: %s\n", usocket);

	char UDPPortStudent[6];			//To store "eeeee"
	char UDPPortRobot[6];			//To store "fffff"

	strncpy(UDPPortRobot, usocket, 6);
	UDPPortRobot[5]=0;	//Ensure null-terminated


	int i;
	for(i=0;i<=4;i++){ UDPPortStudent[i]=usocket[i+6]; }	//copy "eeeee" in usocket to UDPPortStudent
	UDPPortStudent[5]=0;	//Ensure null-terminated
	
	printf("student: %s, robot: %s\n", UDPPortStudent, UDPPortRobot);

	int iUDPPortStudent = atoi(UDPPortStudent);
	int iUDPPortRobot = atoi(UDPPortRobot);

	// Create a UDP socket to send the data
	printf("\nPreparing socket s3 <%d>...", iUDPPortRobot);
	s3 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s3 < 0) {
		printf("Error at socket()\n");
		return 1;
	}
	struct sockaddr_in udpAddr;
	udpAddr.sin_family = AF_INET;
	udpAddr.sin_port = htons(iUDPPortStudent);
	udpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s3, (struct sockaddr *) &udpAddr, sizeof(udpAddr));
	
	//	Prepare the receiver address for the UDP packet
	struct sockaddr_in robotAddr;
	robotAddr.sin_family = AF_INET;
	robotAddr.sin_port = htons(iUDPPortRobot);
	robotAddr.sin_addr.s_addr = inet_addr(robotIP);

	printf("Done\n");

	int num = rand()%4+6;

	printf("num = %d\n", num);

	char numchar[1024];
	sprintf(numchar, "%d", num);

	int t = sendto(s3, numchar, strlen(numchar), 0, (struct sockaddr*) &robotAddr, sizeof(robotAddr));

	char messageBuffer[1024];
	memset(messageBuffer, 0, sizeof(messageBuffer));

	recv(s3, messageBuffer, sizeof(messageBuffer), 0);			//receive char string xxx with length x*10

	printf("char string xxx received = %s\n", messageBuffer);

	for (int i=0; i<5; i++){
		int t1 = sendto(s3, messageBuffer, strlen(messageBuffer), 0, (struct sockaddr*) &robotAddr, sizeof(robotAddr));
		sleep(1);
		printf("UDP packet %d sent\n", i+1);
	}

//EXPERIMENT PART------------------------------------------------------------------------------------------------ 




printf("-----------Experiment part-----------\n");
////////step7
/* Retrieve and print the default buffer size */
/*int rcvBufferSize;
socklen_t sockOptSize;
sockOptSize = sizeof(rcvBufferSize);

if (getsockopt(s2, SOL_SOCKET, SO_RCVBUF, &rcvBufferSize, &sockOptSize) < 0)
printf("getsockopt() failed\n");
else
printf("Initial Receive Buffer Size: %d\n", rcvBufferSize);

char rcvBuff[1024];
sprintf(rcvBuff, "%dbs", rcvBufferSize);	//buffer information to be sent to robot
send(s2, rcvBuff, sizeof(rcvBuff), 0);

printf("Receiving packets in 30 seconds...\n");

long long int nbytes, totalBytes;		//nbytes: bytes received in one loop, totalBytes: all the bytes received in 30 sec
char packetsrecv[rcvBufferSize];

time_t startingTime, currentTime;
int n=0;
do{
	if(nbytes = recv(s2, packetsrecv, sizeof(packetsrecv), 0)){		//receive packets of nbytes bytes
		currentTime = time(NULL);		//record current time
		totalBytes += nbytes;
		n++;
		if(n==1){
			startingTime = currentTime;		//start time = current time for the first packet is sent
		}
			
	}
}while( (currentTime-startingTime) <=30);	//while(current time - start time <= 30sec)
		
printf("Done\n");
printf("Number of received packets: %d, total received bytes: %lld\n", n, totalBytes);
*/

////////step8

// Set buffer size
int rcvbuff;
printf("Set receivebuffer size to ___ bytes: ");
scanf("%d", &rcvbuff);

printf("setting the receive buffer to %d\n", rcvbuff);
rcvbuff = rcvbuff/2;
int res=0;

res = setsockopt(s2, SOL_SOCKET, SO_RCVBUF, &rcvbuff, sizeof(rcvbuff));
if(res == -1)
 printf("Error setsockopt");

// Get buffer size
socklen_t sockOptSize1 = sizeof(rcvbuff);
if (getsockopt(s2, SOL_SOCKET, SO_RCVBUF, &rcvbuff, &sockOptSize1) < 0)
printf("getsockopt() failed\n");
else
printf("Receive Buffer Size is set to: %d\n", rcvbuff);

char rcvBuffSet[1024];		//to store the buffer information we set
sprintf(rcvBuffSet, "%dbs", rcvbuff);	//buffer information to be sent to robot
send(s2, rcvBuffSet, sizeof(rcvBuffSet), 0);

//
printf("Receiving packets in 30 seconds after new receive buffer size...\n");

long long int nbytes1, totalBytes1;		//nbytes1: bytes received in one loop, totalBytes1: all the bytes received in 30 sec
char packetsrecv1[rcvbuff];

time_t startingTime1, currentTime1;
int n1=0;
do{
	if(nbytes1 = recv(s2, packetsrecv1, sizeof(packetsrecv1), 0)){		//send packet of n1 bytes
		currentTime1 = time(NULL);		//record current time
		totalBytes1 += nbytes1;
		n1++;
		if(n1==1){
			startingTime1 = currentTime1;		//start time = current time for the first packet is sent
		}
			
	}
}while( (currentTime1-startingTime1) <=30);	//while(current time - start time <= 30sec)

printf("Done\n");
printf("Number of received packets: %d, total received bytes: %lld\n", n1, totalBytes1);

//EXPERIMENT PART------------------------------------------------------------------------------------------------ 

	close(s2);
	close(s3);

	return 0;
}

