# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/09 12:05:19 by kipouliq          #+#    #+#              #
#    Updated: 2026/03/10 17:19:19 by kipouliq         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_ping

# NAME_BONUS = ft_ping_bonus

SRC = ./srcs/ft_ping.c \
	./srcs/parsing.c \
	./srcs/linked_list.c
		
OBJ = $(SRC:.c=.o)

OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

OBJS_DIR = .objs/

# BONUS_OBJS = $(SRC_BONUS:.c=.o)

CC = cc

FLAGS = -Wall -Wextra -Werror

LINK = $(CC) $(FLAGS) -o $(NAME) $(OBJS) -lm -g3

all : $(NAME)

$(OBJS_DIR)%.o: %.c
	mkdir -p $(@D)
	$(CC) $(FLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(LINK)
	sudo setcap cap_net_raw=ep ./$(NAME)

# bonus : $(NAME_BONUS)

# $(NAME_BONUS) : $(BONUS_OBJS)
# 	$(CC) $(FLAGS) -g3 $(BONUS_OBJS) -o $(NAME_BONUS)

valgrind : $(OBJS)
	$(LINK)
	sudo valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./ft_ping $(ARGS)

clean :
	rm -rf $(OBJS_DIR)

fclean : clean
	rm -f $(NAME)
	rm -rf ./.vagrant
	
re : fclean
	make all

.PHONY : all bonus clean fclean re