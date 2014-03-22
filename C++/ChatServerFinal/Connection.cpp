/*
    Copyright (c) 2013 Danny Beaumont <email>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include "Connection.h"

Connection::Connection()
{
  yes = 1;
}


void* Connection::getSocketAddress(sockaddr* sa)
{
    if(sa->sa_family == AF_INET){
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Connection::serve()
{
  stringstream ss;
  memset(&hints, 0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
  if((rv = getaddrinfo(NULL,PORT,&hints,&ai)) !=0){
   errorMsg = "Select Server: ";
    cout << errorMsg << gai_strerror(rv);
    cout << endl;
    exit(1);
  }
  for(p=ai;p != NULL;p->ai_next){
    listener = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    if(listener < 0){
      continue;
    }// end if
    
    setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    
    if(bind(listener,p->ai_addr,p->ai_addrlen) < 0){
      close(listener);
      continue;
    }
    break;
  }//end for
  
  if(p=NULL){
    errorMsg = "Server selection failed to bind";
    cout << errorMsg << endl;
    exit(2);
  }
  
  freeaddrinfo(ai);
  
  if(listen(listener,10) == -1){
    errorMsg = "listen";
    cout << errorMsg
    << endl;
    exit(3);
  }
  
  FD_SET(listener,&master);
  
  fdmax = listener;
  
  for(;;){
    read_fds = master;
    if(select(fdmax+1,&read_fds,NULL,NULL,NULL) == -1){
      errorMsg = "Select";
      cout << errorMsg
      << endl;
      exit(4);
    }
    
    for(i=0;i<=fdmax;i++){
      if(FD_ISSET(i,&read_fds)){
	if(i == listener){//handle our new connection
	  addrlen = sizeof remoteaddr;
	  newfd = accept(listener,(struct sockaddr*) &remoteaddr,&addrlen);
	  if(newfd == -1){
	    cout << "accept error" <<endl;
	  }else{
	    FD_SET(newfd,&master);
	  if(newfd > fdmax){
	    fdmax = newfd;
	  }
	  
	  ss << "User" << i;
	  rooms[i].room = 0;
	  rooms[i].ipAddress = inet_ntop(remoteaddr.ss_family,getSocketAddress((struct sockaddr*)&remoteaddr),remoteIP,INET6_ADDRSTRLEN);
	  rooms[i].socketNum = newfd;
	  ss >> rooms[i].userName;
	  cout << "New connection from " 
	  << rooms[i].ipAddress
	  << " on socket " << newfd << " username: "<< rooms[i].userName<<" assigned." <<endl;
	  string welcome = "Welcome to Danny's super secret ultra awesome chat program \n\r Press 1 for Kosake\'s house of poop \n\r Press 2 for Ultra awesome room of pr0n \n\r you are currently in the lobby:\n\r" ;
	  //int wlen = strlen(welcome);
	  if((nbytes = send(newfd,welcome.c_str(),welcome.length(),0))=0) FD_CLR(i,&master);
	  
	  }
	}else{
	  //handle data from client
	  int k;
	  for(k =0;k<SIZE;k++) buf[k] = '\0';//clear our buffer
	    nbytes = recv(i,buf,SIZE,0); // recv data from client
	    cout << /*nbytes*/rooms[i].userName << " says "<< i << " : "<< buf << endl;
	    if(nbytes == 0){ 
	      FD_CLR(i,&master);
	      cout << "client on socket: "<< i << " disconnected" << endl;
	      
	    }else if(nbytes == 3){
	      if(int(buf[0]) == 27){
		for(int j=0;j<strlen(buf);j++)
		buf[j] = '\0';
		send(i,"Disconnected \n\r",15,0);
		FD_CLR(i,&master);
		close(i);
	      }
	      int ch = int(buf[0]);
	      switch(ch){
		case 49://ascii 49 is the number 1
		  int countClients =0;
		  for(int j=0;j < 10;j++) if(rooms[j].room == 1)countClients++;//end for
		  if (countClients >= 3){
		    msg = "Unable to  enter the at this time\n\r";
		    send(i,msg.c_str(),msg.length(),0);
		  }else{
		    msg = "Welcome to Kosake\'s house of poop\n\r";
		    send(i,msg.c_str(),msg.length(),0);
		    
		    rooms[i].room = 1;
		    cout <<"Entered room:"<< rooms[i].room << endl;
		  }
		    
		  break;
		
	      }
	    }else{
	      if(buf[0] == '/'){//this is how determine if it's command or not
		cout << "test for /  complete" << endl; 
		cout << buf[1] << endl;
		switch(int(buf[1])){
		  case 84://intentional fallthrough to make case insensative
		  case 116:// /T or /t
		    break;
		  case 87:// this is /W or/w
		  case 119:
		    break;
		  case 76://to leave a room we type /l or /L
		  case 108:
		    rooms[i].room = 0;//back to the lobby
		    
		    ss << rooms[i].userName <<" has left the room";
		    ss >> msg;
		    for(j=0;j<=fdmax;j++){
		      if(FD_ISSET(j,&master)){
			if(j!=listener && rooms[j].room == 1){
			if((send(j,msg.c_str(),msg.length(),0)) == -1){
			  FD_CLR(j,&master);
			  close(j);
			}
			}
		      }
		    }
		    break;
		    
		}
	      }
	    }
	   for(j = 0; j <= fdmax; j++) {
	     
                            // send to everyone!
                            
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
				  ss << "room " << rooms[j].room << ":";
				  ss >> msg;
				  send(j,msg.c_str(),msg.length(),0);
                                    if ((send(j, buf, SIZE, 0)) == -1) {
				      cout << nbytes << endl;
				      FD_CLR(j,&master);
                                       close(j); 
				       
                                    }
                                }
                            }
                        }
	}
      }//end if new connection
    }//end inner for file descriptor
  }//end for(;;)
}

