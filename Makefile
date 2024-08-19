#---------------------------------
#VARIABLES

NAME 	:= webserv
SRC 	:= main.cpp parser.cpp
SUBDIR	:= srcs/
SRCS	:= $(addprefix $(SUBDIR),$(SRC))
OBJ 	:= $(SRCS:.c=.o)
CMP		:= g++
FLAGS 	:= -Werror -Wall -Wextra -g -I lib

#---------------------------------
#FORMATTING AND FUN

clear_line = \033[K
move_up = \033[A
define prettycomp
	@printf "$(1)$(clear_line)\n"
	@$(1)
	@printf "$(move_up)"
endef

RED		:= \033[31m
GREEN 	:= \033[32m
YELLOW	:= \033[33m
BLUE	:= \033[38;5;87m
MAGENTA := \033[35m
CYAN	:= \033[36m
RESET	:= \033[0m

#---------------------------------
#RULES

all : 
#		 g++ -fsanitize=address -g src/main.cpp src/parser.cpp
		g++ -g src/main.cpp src/parser.cpp src/Socket.cpp

# all	: $(NAME)
# 		@echo "$(GREEN)Project built successfully !$(RESET)"
# 		@echo "$(BLUE)Usage: ./cub3d <map>$(RESET)"

$(NAME) : $(OBJ)
		@$(CC) $(FLAGS) $(OSFLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp
		$(CMP) -c $(FLAGS) -o $@ $<

clean :
		@rm -f $(OBJ)

fclean :
		@rm -f $(NAME) $(OBJ)

re : fclean all

.PHONY : clean fclean re hardclean
