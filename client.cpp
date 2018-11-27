//Max Hirata
//pa4_client.cpp

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>  // size_t, ssize_t
#include <sys/socket.h> // socket funcs
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // htons, inet_pton
#include <unistd.h>     // close
#include <netdb.h>

#include <ctype.h>
using namespace std;

int main(int argc, char* argv[])
{
  int client, server;
  //int portNum = 11000;
  char* IPAddr = argv[1];
  short portNum = stoi(argv[2]);
  //bool isExit = false;
  //int bufsize = 1024;


  //1) create a  TCP client
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

  //2) establich a connection
  // Assume we are connecting to 10.124.70.20, port 11002
//char *IPAddr = "10.124.72.20";
//unsigned short servPort = 4567;
// Convert dotted decimal address to int
unsigned long servIP;
int status = inet_pton(AF_INET, IPAddr, (void *) &servIP); 
if (status <= 0) exit(-1);
// Set the fields
struct sockaddr_in servAddr;
servAddr.sin_family = AF_INET; // always AF_INET
servAddr.sin_addr.s_addr = servIP;
servAddr.sin_port = htons(portNum);

//status = connect(sock, (struct sockAddr *)&servAddr, sizeof(servAddr));

status = connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
if(status < 0)
{
  cout << "ERROR: Failed to connect...." << endl;
  exit(-1);
}
else
	cout << "Connected to the server port number: " << portNum << endl;


int bufsize = 1024;
char buffer[bufsize];

//cout << "Checking connection from server" << endl;
//recv(sock,buffer,bufsize, 0);
//cout << "Connection is Successful " << endl;

  //3) communicate


//GAME SETUP
bool done = false;

cout << "Welcome to Treasure Hunt!" << endl;
cout << "Enter Player's name: ";
char name[50];
cin >> name;
send(sock, name, 50, 0);
int turnCount = 0;

cout << endl << endl;

while(!done)
{
  bool valid = false;
  turnCount++;

  cout << "Turn: " << turnCount << endl;
  do{
    long x, y;
    cout << "Enter a guess (x,y):";
    cin >> x;
    cin >> y;
    if(x > 100 || x < -100 || y > 100 || y < -100)
    {
      cout << "\nNot a valid entry....try again..." << endl;
      valid = false;
    }
    else
    {
      long coord_x = htonl(x);
      long coord_y = htonl(y);
      int bytesSent1 = send(sock, (void *) &coord_x, sizeof(long), 0);
      if(bytesSent1 != sizeof(long)) exit(-1);
      int bytesSent2 = send(sock, (void *) &coord_y, sizeof(long), 0);
      if(bytesSent2 != sizeof(long)) exit(-1);
      valid = true;
    }
  }while(!valid);


    int bytesLeft = sizeof(long);
    long networkInt;
    char *bp = (char*) &networkInt;
    while(bytesLeft)
    {
      int bytesRecv = recv(sock, bp, bytesLeft, 0);
      if(bytesLeft <= 0) exit(-1);
      bytesLeft = bytesLeft - bytesRecv;
      bp = bp + bytesRecv;
    } 

    long distance = ntohl(networkInt);

    if(distance == 0)
    {
      cout << "Congratulations! You found the Treasure!!!" << endl;
      cout << "It took you " << turnCount << " turns to find the treasure." << endl;
      done = true;
    }
    else
    {
      cout << "Distance to treasure: " << distance << " ft." << endl;
    }
    cout << endl;
}



  //4) close connection 
  cout << "connection terminated" << endl;
  close(sock);

  return 0;
}





