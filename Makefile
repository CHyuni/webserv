BLUE := \033[0;36m
RESET := \033[0m

RED_START := 255
GREEN_START := 0
RED_END := 0
GREEN_END := 255

PROGRESS = 0
COUNT = $(words $(SRCS))
DONE = 0
BAR_FILLED = 0
BAR_EMPTY = 60

C++ := c++
CFLAGS := -Wall -Wextra -Werror -std=c++98 -g
RM := rm -f

NAME := webserv

UTILITY = ./Utility/Utility.cpp

CONFIGURE = ./Configure/Conf.cpp ./Configure/Location.cpp ./Configure/ServerBlock.cpp
REQUEST = ./Request/debug.cpp ./Request/Request.cpp ./Request/RequestGetter.cpp ./Request/RequestParse.cpp ./Request/RequestStatus.cpp ./Request/RequestUtility.cpp ./Request/RequestConfig.cpp
SOCKET = ./Socket/Epoll.cpp ./Socket/main.cpp

RESPONSE = ./Response/RequestHandler.cpp ./Response/RequestValidateCheck.cpp
INC_DIR := .
SRCS := $(CONFIGURE) $(REQUEST) $(SOCKET) $(METHOD) $(RESPONSE) $(UTILITY)
OBJS := $(SRCS:.cpp=.o)

.PHONY : all start clean fclean re

all : $(NAME)

$(NAME) :
	@echo
	@make start --no-print-directory
	@$(C++) $(CFLAGS) $(OBJS) -iquote $(INC_DIR) -o $(NAME)
	@echo "  Completed! "
	chmod +x start.sh
	bash start.sh

start : $(OBJS)

%.o : $(SRC_DIR)/%.cpp
	@$(C++) $(CFLAGS) -c $< -o $@
	@$(eval DONE := $(shell echo $$(($(DONE) + 1))))
	@$(eval PROGRESS := $(shell echo $$(($(DONE) * 100 / $(COUNT)))))
	@RED=$$(($(RED_START) + $(PROGRESS) * ($(RED_END) - $(RED_START)) / 100)); \
	GREEN=$$(($(GREEN_START) + $(PROGRESS) * ($(GREEN_END) - $(GREEN_START)) / 100)); \
	COLOR="\033[38;2;$$RED;$$GREEN;0m"; \
	printf "\033[2A\033[K  Currently working on: $(BLUE)$<$(RESET)\n"; \
	printf "$$COLOR\033[K    %3d$(RESET)%%│$$COLOR" $(PROGRESS); \
	BAR_FILLED=$$(($(PROGRESS) / 5 * 3)); \
	BAR_EMPTY=$$((60 - $$BAR_FILLED)); \
	for i in $$(seq 1 $$BAR_FILLED); do \
		printf "█"; \
	done; \
	for i in $$(seq 1 $$BAR_EMPTY); do \
		printf " "; \
	done; \
	printf "$(RESET)│\n"

clean :
	@$(RM) $(OBJS)
	@echo "  All object files are deleted."

fclean :
	@$(RM) $(OBJS)
	@$(RM) $(NAME)
	@echo "  All object files and $(BLUE)$(NAME)$(RESET) are deleted."

re :
	@make fclean --no-print-directory
	@echo
	@make all --no-print-directory

