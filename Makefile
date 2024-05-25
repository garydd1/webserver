# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/12 11:25:31 by dgarizad          #+#    #+#              #
#    Updated: 2024/05/25 22:17:18 by dgarizad         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


################################################################################
#                                     CONFIG                                   #
################################################################################

NAME = webserv

HEADERS = includes/webserv.hpp  includes/Master.hpp  includes/VHost.hpp \
includes/FileParse.hpp includes/Client.hpp
OBJDIR = ./obj
FLAGS = -Wall -Wextra -Werror -std=c++98

CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
RM		    := rm -rf

################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

# SRCS =  $(wildcard srcs/*.cpp)
SRCS = main.cpp Master.cpp  Epoll.cpp utils.cpp VHost.cpp  FileParse.cpp Client.cpp
SRCS := $(addprefix srcs/, $(SRCS))
# SRCS = srcs/main.cpp srcs/ConfParse.cpp srcs/Master.cpp srcs/utils.cpp
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
# OBJS = $(SRCS:srcs/%.cpp=$(OBJDIR)/%.o)
OS = #HERE WE DISCOVER THE OS
$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@g++ -c $< -o $@  #$(FLAGS)





################################################################################
#                                  Makefile  objs                              #
################################################################################

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	@g++ -o $(NAME) $(OBJS) #$(FLAGS)
	@echo "$(GREEN)$(NAME) executable has been created!$(CLR_RMV)"

clean:
	@$(RM) $(OBJDIR)
	@echo "$(YELLOW)$(NAME) objects have been removed$(CLR_RMV)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)$(NAME) executable has been removed!$(CLR_RMV)"