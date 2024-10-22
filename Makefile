SRCS			= auto_index.cpp error_responce.cpp running_servers.cpp main.cpp parser.cpp Socket.cpp handle_connection.cpp Request.cpp

SRCSPATH		= src/
BINPATH			= bin/

OBJS			= $(addprefix $(BINPATH), $(SRCS:.cpp=.o))

CC				= c++
CFLAGS			=  -I. -g -std=c++17 -fsanitize=address -Wall -Wextra -Werror 

NAME			=	webserver

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CC) $(CFLAGS) $(OBJS) -o $(NAME) 	

$(BINPATH)%.o: $(SRCSPATH)%.cpp | $(BINPATH)
				$(CC) $(CFLAGS) -c $< -o $@

$(BINPATH):
				@mkdir -p $(BINPATH)

clean:
				@rm -f $(OBJS) $(OBJS_BONUS) test_demo
				@rm -rf $(BINPATH)

fclean:			clean
				@rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re bonus

