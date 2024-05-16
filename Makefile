# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/12 11:25:31 by dgarizad          #+#    #+#              #
#    Updated: 2024/05/15 12:44:27 by vcereced         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


################################################################################
#                                     CONFIG                                   #
################################################################################

NAME = webserv

HEADERS = includes/webserv.hpp  includes/ConfParse.hpp
OBJDIR = ./obj


################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

# SRCS =  $(wildcard srcs/*.cpp)
SRCS = srcs/main.cpp srcs/ConfParse.cpp
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
# OBJS = $(SRCS:srcs/%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	g++ -Wall -Wextra -Werror -std=c++98 -c $< -o $@





################################################################################
#                                  Makefile  objs                              #
################################################################################

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
		g++ -Wall -Wextra -Werror -std=c++98 -o $(NAME) $(OBJS)

################################################################################
#                                  VICTOR LAB                                  #
################################################################################

fvic:
	rm vic VicParseImproved.o vicmain.o

vic:
	g++ -Wall -Wextra -Werror VicParseImproved.cpp vicmain.cpp -o vic && ./vic