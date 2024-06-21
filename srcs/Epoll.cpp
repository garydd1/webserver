/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 19:41:13 by vcereced          #+#    #+#             */
/*   Updated: 2024/06/21 08:50:08 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Master.hpp"

#include <stdio.h>
#include <stdlib.h>



int Master::setSockets(std::set<int> &ports)
{
	std::set<int>::iterator it = ports.begin();
	for (; it != ports.end(); ++it)
	{
		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0)
			ft_error("Error creating socket");

		// Set socket to non-blocking
		fcntl(serverSocket, F_SETFL, O_NONBLOCK);

		struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(*it);
		serverAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
			ft_error("Error binding socket");

		if (listen(serverSocket, 10) < 0)
			ft_error("Error listening on socket");

		_ListenSockets.push_back(serverSocket);
		std::cout << GREEN << "Socket " << serverSocket << " listening on port " << *it << RESET << std::endl;
	}
	return (0);
}

/**
 * @brief Creates the epoll set and adds the server sockets to it.
 * epoll_create1(0) creates a new epoll instance.
 * @return int 
 */
int Master::setEvents()
{
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1)
        ft_error("Error creating epoll");

    std::vector<int>::iterator it = _ListenSockets.begin();
    for (; it != _ListenSockets.end(); it++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET; // Edge-triggered mode
        ev.data.fd = *it;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, *it, &ev) == -1)
            ft_error("Error adding socket to epoll");
    }
    return (0);
}

/**
 * @brief Accepts a connection on a server socket and adds the client socket to the epoll set.
 * Creates a Client object and adds it to the _clientsMap.
 * 
 * @param socketToAccept 
 * @return int 
 */
int Master::clientAccept(int socketToAccept)
{
    Connection  client;
	sockaddr_in clientAddr;
    socklen_t   clientAddrSize = sizeof(clientAddr);
    int         clientSocket;
    
    clientSocket = accept(socketToAccept, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket < 0)
       return(ft_error("Error accepting connection"));
    std::cout << GREEN << "Client socket: " << clientSocket << " accepted connection on listen socket " << socketToAccept << " from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << RESET << std::endl;
    // Set client socket to non-blocking
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientSocket;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, clientSocket, &ev) == -1)
        return(ft_error("Error adding client socket to epoll"));
	
    _clientSockets.push_back(clientSocket);
    client.setClientData(clientSocket, clientAddr, clientAddrSize, ev);
    _clientsMap[clientSocket] = client;
    return (0);
}

/**
 * @brief Here we read from the client socket and store the buffer in the Client object.
 * 
 * @param clientSocket 
 * @return int 
 */
void Master::clientRead(int clientSocket)
{
    char buffer[2048];//????????????????????????????????????????????limitante para el upload???????????????
    int bytesRead = read(clientSocket, buffer, 2048);
    if (bytesRead < 0)
        throw std::runtime_error("clientRead: Error reading from socket");  //REMOVE CLIENT SOCKET FROM EPOLL SET AND CLOSE SOCKET!

    if (bytesRead == 0)
    {
        //Remove the client socket from the epoll set, close the socket and remove it from the clientSockets vector
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, clientSocket, NULL);
        close(clientSocket);
        _clientSockets.erase(std::remove(_clientSockets.begin(), _clientSockets.end(), clientSocket), _clientSockets.end());
        // ft_error("Client disconnected"); //Is this an error? or just a message? ... maybe a message. HANDLE IT!!!
        std::cout << RED << "Client disconnected" << RESET << std::endl;
         // This is not an error, it's just a message HANDLE THIS!!!
    }
    else //store the buffer in the Client object
    {
        buffer[bytesRead] = '\0';
        _clientsMap[clientSocket].setBuffer(buffer);
    } 
}

void Master::manageConnections(struct epoll_event *events, int nev)
{
    int socketToAccept;
    
    for (int i = 0; i < nev; ++i)
    {
        socketToAccept = events[i].data.fd;

        if (std::find(_ListenSockets.begin(), _ListenSockets.end(), socketToAccept) != _ListenSockets.end())
        {
            if (clientAccept(socketToAccept) < 0)
                ft_error("Error accepting client");
        }
        else if (std::find(_clientSockets.begin(), _clientSockets.end(), socketToAccept) != _clientSockets.end())
        { 
            std::cout << "Client socket: " << socketToAccept << " being managed..." << std::endl;
            try {
                manageConnection(_clientsMap[socketToAccept]);
            } catch (std::exception &e) {
                std::cerr << RED << "startEventLoop: " << e.what() << RESET << std::endl;
                _clientsMap[socketToAccept].serveErrorPage();
            }
            std::cout << "Client socket: " << socketToAccept << " being disconected and erased..." << std::endl;
            close(socketToAccept);//???????????????????????????????????????????''
            _clientSockets.erase(std::remove(_clientSockets.begin(), _clientSockets.end(), socketToAccept), _clientSockets.end());//?????????????

            //epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, socketToAccept, NULL);
            //close(socketToAccept);
            //_clientSockets.erase(std::remove(_clientSockets.begin(), _clientSockets.end(), socketToAccept), _clientSockets.end());

        }
    }
}

/**
 * @brief Main event loop. If a server socket receives a connection, it will call clientAccept. 
 * If a client socket is ready to read, it will call clientRead.
 * 
 * @return int 
 */
int Master::startEventLoop()
{
    struct epoll_event events[MAX_EVENTS];
    int nev;

    while (true)
    {
        printWaitConsole();
        
        nev = epoll_wait(_epoll_fd, events, MAX_EVENTS, 300); 
        
        if (nev == -1)
            ft_error("Error in epoll_wait");
        else if (nev > 0)
            manageConnections(events, nev);
    }
    return (0);
}


            // if (std::find(_ListenSockets.begin(), _ListenSockets.end(), socketToAccept) != _ListenSockets.end())
            // {
            //     if (clientAccept(socketToAccept) < 0)
            //         ft_error("Error accepting client");
            // } else //  A client socket is ready to read
            // {
            //     try {
            //         manageConnection(socketToAccept);
            //     } catch (std::exception &e) {
            //         std::cerr << e.what() << std::endl;
            //         close(socketToAccept);
            //         _clientSockets.erase(std::remove(_clientSockets.begin(), _clientSockets.end(), socketToAccept), _clientSockets.end());
            //     }
            // }
            // check for a write event
            // if (events[i].events & EPOLLOUT)
            // {
            //     // write to the client
            // }
