NAME = bot

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRC = sources/main.cpp sources/Bot.cpp sources/Game.cpp

OBJS = $(SRC:.cpp=.o)

.cpp.o :
	${CC} ${CFLAGS} -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

re :  fclean all

clean :
	rm -rf $(NAME) $(OBJS)
fclean : clean