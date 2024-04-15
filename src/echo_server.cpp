#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_EVENTS 1000
#define BUFFER_SZIE 32
void ChechError(int err, const char* msg)
{
  if(err == -1){
    std::cout<< "Something wrong happened " << msg << "\n";
  }
}


int Socket(const char* addr, int port)
{
  struct sockaddr_in address;
  bzero(&address, sizeof ( address ) );
  address.sin_family = PF_INET;
  inet_pton(AF_INET, addr, &address.sin_addr);
  address.sin_port = htons(port);

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  ChechError(listenfd, "create listenfd failed");
  ChechError(bind(listenfd, (sockaddr*)&address, sizeof(address)), "bind failed");
  ChechError(listen(listenfd, 5), "listenfd listen failed");
  return listenfd;
}
void setnonblock(int fd)
{
  int old_opt = fcntl(fd, F_GETFL);
  int new_opt = old_opt | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_opt);
}
void addfd(int epollfd, int fd)
{
  epoll_event event;
  event.events = EPOLLIN | EPOLLOUT;
  event.data.fd = fd;
  setnonblock(fd);
  ChechError(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event), "add fd to epoll failed");
}


int main(int argc, char ** argv)
{
  if(argc <= 2)
  {
    std::cout << "Usage: " << argv[0] << "ip_address port_number \n";
  }
  int listenfd = Socket(argv[1], atoi(argv[2]));
  int epollfd = epoll_create(5);
  addfd(epollfd, listenfd);

  while(true)
  {
    //首先需要提前为event数组分配空间
    epoll_event event[MAX_EVENTS];
    int ret = epoll_wait(epollfd, event, MAX_EVENTS, -1);
    char buf[BUFFER_SZIE];
    for(int i=0; i<ret; ++i)
    {
      if(event[i].data.fd == listenfd)
      {
        struct sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);
        int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
        ChechError(connfd, "accept from socket failed");
        addfd(epollfd, connfd);
        std::cout << "Got connection from "<< ntohs(client_address.sin_port) << '\n';
      }
      else if((event[i].events & EPOLLIN) && (event[i].events & EPOLLOUT))
      {
        memset(buf, 0, BUFFER_SZIE);
        int r = recv(event[i].data.fd, buf, BUFFER_SZIE - 1, 0);
        ChechError(r, "recv from socket failed");
        int w = send(event[i].data.fd, buf, r, 0);
        ChechError(w, "send to socket failed");
      }
    }
  }
  close(epollfd);
  close(listenfd);
}