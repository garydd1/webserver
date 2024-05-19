/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 21:31:14 by dgarizad          #+#    #+#             */
/*   Updated: 2024/05/19 17:40:14 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//HERE WE WILL INCLUDE ALL THE HEADER FILES NEEDED FOR THE WEB SERVER
#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/event.h> // Include this for kqueue
#include <sys/time.h>  // Include this for kqueue
#include <set>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"


enum nums {
	LISTEN = 0,
	SERVER_NAME = 1,
	LOCATION = 2,
	ROOT = 0,
	INDEX = 1
};

typedef struct s_location {
	std::string					uri;
	std::string					root;
	std::vector<std::string>	index;
}t_location;

typedef struct s_server {
    std::vector<std::string>	server_name;
    std::string					serverName;
    int 						listen;
    int 						port;
    std::string					errorLogs;
    std::string					workerConnections;
    std::vector<t_location>		locations;

} t_server;

typedef struct s_fileParse {
	int							test;
	std::set<int>				ports;
	std::string					errorLog;
	std::string					workerConnections;
	std::vector<t_server>		serverData;
}t_fileParse;


#include "FileParse.hpp"
#include "VHost.hpp"
#include "Master.hpp"

int ft_error(std::string msg);
#endif