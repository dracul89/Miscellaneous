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
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string.h>
#include <netdb.h>
#include <cstdlib>
#include <unistd.h>
#include <sstream>

#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#define PORT "6667"  // typical irc port
#define SIZE 256

using namespace std;
class Connection
{
private:
  typedef struct {//things I want to know about the client
  int socketNum;
  int room;
  string userName;
  string ipAddress;
  }client;
  fd_set master;
  fd_set read_fds;
  client rooms[50];// no more than 50 connections to the server
  int fdmax;
  int listener;
  int newfd;
  class sockaddr_storage remoteaddr;
  socklen_t addrlen;
  char buf[SIZE];
  int nbytes;
  char remoteIP[INET6_ADDRSTRLEN];
  int yes;
  string errorMsg;
  string msg;
  class addrinfo hints, *ai, *p;
    
    int rv;
    int i;
    int j;
protected:
public:
  Connection();
  void *getSocketAddress(struct sockaddr* sa);
  void serve();
};

#endif // CONNECTION_H
