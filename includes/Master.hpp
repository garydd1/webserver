/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:11:34 by dgarizad          #+#    #+#             */
/*   Updated: 2024/06/23 19:26:17 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MASTER_HPP
# define MASTER_HPP
# define MAX_CLIENTS 100
#include "webserv.hpp"
#include "FileParse.hpp"
#include "VHost.hpp"
#include "Connection.hpp"
#include "RequestParser.hpp"
#include <sys/types.h>
#include <sys/socket.h>


class VHost;
class Connection;

class Master
{
	public:
		Master();
		~Master();
		Master(Master const &src);
		Master &operator=(Master const &src);

		int createVHosts(FileParse &config);
		
		//NETWORK METHODS
		int  setSockets(std::set<int> &ports);
		int  setEvents();
		int  startEventLoop();
		int  clientAccept(int serverSocket);
		void deleteConnection(int SocketAccepted);
		//void processRequest(Connection &connection, RequestParser &request);
		
		void manageConnection(Connection &connection);
		void manageConnections(struct epoll_event *events, int nev);

		void setErrPages(std::map<int, std::string> errPagesMap);

		//FINDS AND RETURNS THE VHOST OBJECT FOR A GIVEN SERVER NAME
		VHost &getVHost(std::string serverName, int port);
		VHost &assignVHost(std::string hostport);
		std::map<int, std::string> &getErrPages(void);
		

		//AUXILIARY FUNCTIONS
		void printServerNames();
		//int  servePage(Connection &connection, const std::string &path);
		
	private:

		std::vector<int> _ListenSockets;

		std::vector<VHost> _vhosts;
		std::map<std::string, VHost> _vhostMap;
		std::map<int, Connection> _clientsMap;
		std::map<int, std::string> 		_errPageMap;
		int _kq;
		int _epoll_fd;
};


#endif