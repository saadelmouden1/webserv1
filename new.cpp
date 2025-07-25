#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <poll.h>


#define PORT 8080

int main()
{
	int server_fd, client_fd;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	int opt = 1;
	std::cout<<"jjj\n";
	
	if((server_fd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("lol");
		return 1;
	}
	std::cout<<"kkk\n";
	if(server_fd == 0)
	{
		perror("socket fail");
		return 1;
	}

	if(setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR, &opt,sizeof(opt))<0)
	{
		perror("setsockopt");
		return 1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port= htons(PORT);

	if(bind(server_fd, (struct sockaddr *)&address ,sizeof(address)) <0)
	{
		perror("bind failed");
		return 1;

	}

	if(listen(server_fd,SOMAXCONN) < 0)
	{
		perror("listen");
		return 1;
	}

	std::cout<<"LIsten on port "<<PORT<<" ...\n";	

	//STIP POLL
	std::vector<pollfd> poll_fds;
	pollfd server_poll;
	server_poll.fd = server_fd;
	server_poll.events = POLLIN;
	poll_fds.push_back(server_poll);

	while(true)
	{
		int ready = poll(poll_fds.data(),poll_fds.size(),-1);
		if(ready <0 )
		{
			perror("poll faild");
			return 1;
		}
		
		for(size_t i = 0;i < poll_fds.size();++i)
		{
			if(poll_fds[i].revents & POLLIN)
			{
				if(poll_fds[i].fd = server_fd)
				{
					int client_fd = accept(server_fd,NULL,NULL);
					//fcntl(client_fd,F_SETFL,O_NONBLOCK);
					if(client_fd < 0)
					{
						perror("accept");
						continue;
					}
					std::cout<<"client connected : "<<client_fd<<"\n";
					pollfd client_poll;
					client_poll.fd = client_fd;
					client_poll.events = POLLIN;
					poll_fds.push_back(client_poll);
				}
				else
				{
					char buffer[4096] = {0};
					int bytes = read(poll_fds[i].fd,buffer, sizeof(buffer));
					if(bytes <= 0)
					{
						std::cout<<"client disconecte :"<<poll_fds[i].fd<<"\n";
						close(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
						--i;
						continue;
					}
					std::cout <<"read "<<bytes<< " bytes fom client\n "<<buffer<<"\n";

				std::cout <<"rtequest from "<<poll_fds[i].fd<<":\n"<<buffer<<"\n";
				std::string response=
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length : 46\r\n"
					"\r\n"
					"<html><body><h1>hello webserv?</h1></body></html>";
				send(poll_fds[i].fd,response.c_str(),response.size(),0);
				close(poll_fds[i].fd);
				poll_fds.erase(poll_fds.begin() + 1);
				--i;

				}
			}
		}


	}
	
	
	close(server_fd);
	return 0;

}
