# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/09 12:05:19 by kipouliq          #+#    #+#              #
#    Updated: 2026/03/18 13:33:02 by kipouliq         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_ping

SRC = ./srcs/ft_ping.c \
	./srcs/parsing.c \
	./srcs/linked_list.c \
	./srcs/network.c \
	./srcs/packet_handling.c \
	./srcs/print.c \
	./srcs/utils.c \
	./srcs/error.c
		
OBJ = $(SRC:.c=.o)

OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

OBJS_DIR = .objs/

CC = cc

FLAGS = -Wall -Wextra -Werror -g

LINK = $(CC) $(FLAGS) -o $(NAME) $(OBJS) -lm

all : $(NAME)

$(OBJS_DIR)%.o: %.c
	mkdir -p $(@D)
	$(CC) $(FLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(LINK)
	sudo setcap cap_net_raw=ep ./$(NAME)

valgrind : $(OBJS)
	$(LINK)
	sudo valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --track-origins=yes ./ft_ping $(ARGS)

clean :
	rm -rf $(OBJS_DIR)

fclean : clean
	rm -f $(NAME)
	rm -rf ./.vagrant
	
re : fclean
	make all

.PHONY : all bonus clean fclean re valgrind