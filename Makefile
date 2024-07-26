#---------------------------------
#VARIABLES

NAME 	:= webserv
SRC 	:= main.c
SUBDIR	:= srcs/
SRCS	:= $(addprefix $(SUBDIR),$(SRC))
OBJ 	:= $(SRCS:.c=.o)
LIBFT_A	:= lib/libft/libft.a
CMP		:= gcc
FLAGS 	:= -Werror -Wall -Wextra -g -I lib
# OS 		:= $(shell uname -m)


#---------------------------------
#OS CHECK

# ifeq ($(OS), arm64)
# 	OSFLAGS = -lglfw -L"/opt/homebrew/Cellar/glfw/3.3.8/lib/"
# else ifeq ($(OS), x86_64)
# 	OSFLAGS	= -lglfw -L"/Users/omathot/.brew/opt/glfw/lib/"
# else
# 	$(error Unsupported architecture: $(OS))
# endif

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
		g++ -fsanitize=address -g src/main.cpp src/parser.cpp

# all	: $(NAME)
# 		@echo "$(GREEN)Project built successfully !$(RESET)"
# 		@echo "$(BLUE)Usage: ./cub3d <map>$(RESET)"

$(NAME) : $(OBJ) $(LIBFT_A)
		@echo "$(CYAN)Creating the executable...$(RESET)"
		@$(CC) $(FLAGS) $(OSFLAGS) $(OBJ) $(LIBFT_A) -o $(NAME)

%.o : %.c
		@echo "$(CYAN)Compiling...$(RESET) $<"
		$(call prettycomp, $(CMP) -c $(FLAGS) -o $@ $<)

$(LIBFT_A) :
		@echo "$(BLUE)Building libft library...$(RESET)\n"
		$(call prettycomp, @make -C lib/libft)

clean :
		@rm -f $(OBJ) ./lib/libft/libft.a
		@echo "$(GREEN)Cleaned up the artifacts !$(RESET)"

fclean :
		@rm -f $(NAME) $(OBJ)
		@echo "$(MAGENTA)Cleaned up executable !$(RESET)"

hardclean :
		@make fclean
		cd ./lib/libft && make clean
		cd ./lib/libft && make fclean
		@echo "$(MAGENTA)Cleaned up all built files!$(RESET)"

re : fclean all

.PHONY : clean fclean re hardclean