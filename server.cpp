//Max Hirata
//pa4_serverM.cpp


//This version of sever works for multiple clients
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>  // size_t, ssize_t
#include <sys/socket.h> // socket funcs
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // htons, inet_pton
#include <unistd.h>     // close
#include <string>
#include <fstream>
#include <pthread.h>
#include <ctime>
#include <time.h>
#include <math.h>
using namespace std;

const int MAX_COORDINATE = 100;
const int MIN_COORDINATE = -100;
const int MAXPENDING = 5;

struct player{
  string name;
  int turnCount = 0;
};

struct ThreadArgs{
	int clientSock; //Socket to communicate with clients
};

void *threadMain(void *args);

void processClient(int clientSock);

int main()
{
  //char *IPAddr = "10.124.72.20";
  unsigned short servPort = 11099;
  
  struct sockaddr_in servAddr;

  //! create TCP socket
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock < 0)
  {
    cout << "ERROR: creating a Socket failed....." << endl;
    exit(-1);
  }
  else
  {
    cout << "Socket Created...." << endl; 
  } 

  // Set the fields
// INADDR_ANY is a wildcard for any IP address 
  servAddr.sin_family = AF_INET; // always AF_INET 
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servAddr.sin_port = htons(servPort);

  int status = bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if(status < 0)
  {
    cout << "ERROR with bind....." << endl;
    exit(-1);
  }
  else
  	cout << "binding Ok" << endl;



  //step 3: set socket to listen
  status = listen(sock, MAXPENDING);
  if(status < 0)
  {
    cout << "ERROR with listen" << endl;
    exit(-1);
  }
  else
  	cout << "listening Ok" << endl;

  //Handling Multiple Clients Sequentailly 
  while(true)
  {
  	//step 4: Accept connection

  	struct sockaddr_in clientAddr;
  	socklen_t addrLen = sizeof(clientAddr);
  	int clientSock = accept(sock, (struct sockaddr *) &clientAddr, & addrLen);
  	if(clientSock < 0)
  	{
  		cout << "Error with accept...." << endl;
  		exit(-1);
  	}

  	//creating initialize argument struct
  	struct ThreadArgs* threadArgs = new ThreadArgs;
  	threadArgs -> clientSock = clientSock;


  	//creating thread 
    pthread_t threadID;
    status = pthread_create(&threadID, NULL, threadMain, (void *)threadArgs);

    if(status != 0)
    {
      cout << "Error creating client thread..."  << endl;
      exit(-1);
    }

  	//step 5: Communicate

  	//step 6: close communication
    
  }


  return 0;
}

void processClient(int clientSock)
{
  int clientCount = 1;
  //bool done = false;
  int bufsize = 1024;
  char buffer[bufsize];

  srand(time(NULL));
  //long treasure_x = rand()% MAX_COORDINATE + MIN_COORDINATE;
  //long treasure_y = rand()% MAX_COORDINATE + MIN_COORDINATE;

  long treasure_x = 50;
  long treasure_y = 35;

  cout << "treasure is at: (" << treasure_x << " , " << treasure_y << ")" << endl;

    while (clientSock > 0) 
    {
      bool done = false;
      int turnCount = 0;
        //strcpy(buffer, "=> Server connected...\n");
        //send(clientSock, buffer, bufsize, 0);
        //cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << endl;
        //cout << "\n=> Enter # to end the connection\n" << endl;
      char name[50];
      recv(clientSock, name, 50, 0);
      //try to store name to struct or something...
      cout << "PLayer: " << name << endl;

      while(!done)
      {
        int bytesLeft = sizeof(long);
        long networkInt;
        char *bp1 = (char *)&networkInt;
        while(bytesLeft)
        {
          int bytesRecv = recv(clientSock, bp1, bytesLeft, 0);
          if(bytesRecv <= 0) exit(-1);
          bytesLeft = bytesLeft - bytesRecv;
          bp1 = bp1 + bytesRecv;
        }
        long client_x = ntohl(networkInt);


        bytesLeft = sizeof(long);
        char *bp2 = (char *)&networkInt;

         while(bytesLeft)
        {
          int bytesRecv = recv(clientSock, bp2, bytesLeft, 0);
          if(bytesRecv <= 0) exit(-1);
          bytesLeft = bytesLeft - bytesRecv;
          bp2 = bp2 + bytesRecv;
        }
        long client_y = ntohl(networkInt);

        cout << "Client: (" << client_x << " , " << client_y << ")" << endl;
        long x_s = ((treasure_x - client_x)*(treasure_x - client_x));
        //cout << "x^2 = " << x_s << endl;
        long y_s =  ( (treasure_y - client_y) * (treasure_y - client_y));
        //cout << "y^2 = " << y_s << endl; 
        
        long distance = sqrt( (  (treasure_x - client_x)*(treasure_x - client_x) ) + ( (treasure_y - client_y) * (treasure_y - client_y)) );
        cout << "Distance away: " << distance << endl;
        long network_distance = htonl(distance);
        int bytesSent = send(clientSock, (void *) &network_distance, sizeof(long), 0);
        if(bytesSent != sizeof(long)) exit(-1);

        if(distance == 0)
        {
          done = true;
        }
        //start game
      }
    }
}

void *threadMain(void *args)
{
  struct ThreadArgs *threadArgs = (struct ThreadArgs *) args;
  int clientSock = threadArgs->clientSock;

  delete threadArgs;

  //communicate with client
  processClient(clientSock);

  //Reclaim resources before finishing
  pthread_detach(pthread_self());
  close(clientSock);

  return NULL;
}




