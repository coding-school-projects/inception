NAME := ircserv
CC := c++
CFLAGS := -Wall -Wextra -Werror -std=c++17 -g -Iinclude -DDEBUG

SRCS = srcs/main.cpp srcs/Client.cpp srcs/Server.cpp srcs/Channel.cpp \
		srcs/handleClients.cpp \
		srcs/commands/client_commands/Pass.cpp \
		srcs/commands/client_commands/Nick.cpp \
		srcs/commands/client_commands/User.cpp \
		srcs/commands/client_commands/Quit.cpp \
		srcs/commands/client_commands/Lusers.cpp \
		srcs/commands/client_commands/List.cpp \
		srcs/commands/client_commands/Names.cpp \
		srcs/commands/client_commands/PrivMsg.cpp \
		srcs/commands/client_commands/Who.cpp \
		srcs/commands/client_commands/Whois.cpp \
		srcs/commands/client_commands/CapLs.cpp \
		srcs/commands/client_commands/Ping.cpp \
		srcs/commands/client_commands/Pong.cpp \
		srcs/commands/channel_commands/Join.cpp \
		srcs/commands/channel_commands/Part.cpp \
		srcs/commands/channel_commands/Mode.cpp \
		srcs/commands/channel_commands/Invite.cpp \
		srcs/commands/channel_commands/Kick.cpp \
		srcs/commands/channel_commands/Topic.cpp \

OBJ_DIR := obj
	
OBJS := $(patsubst srcs/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

GREEN = \033[0;32m
YELLOW = \033[0;33m
CYAN_B = \033[1;36m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "$(GREEN)Created $(NAME)$(RESET)"

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN_B)Compiling... $<$(RESET)"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)Removed Object files$(RESET)"

fclean: clean
	rm -rf $(NAME)
	@echo "$(YELLOW)Removed $(NAME)$(RESET)"

re: fclean all

.PHONY: all clean fclean re