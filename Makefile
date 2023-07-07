
CLS_DIR		:= cls/
CLS_FILES	:= Server.cpp \
			   AServerCluster.cpp \
			   ClusterWeb.cpp

CLS			:= $(addprefix $(CLS_DIR), $(CLS_FILES))

SRC_DIR		:= src/
SRC_FILES	:= main.cpp

SRCS		:= $(addprefix $(SRC_DIR), $(SRC_FILES))

OBJS		:= $(CLS:.cpp=.o) $(SRCS:.cpp=.o)

CLS_INCL	:= cls/
SRC_INCL	:= inc/

INCL_DIR	:= $(CLS_INCL) $(SRC_INCL)
INCLS		:= $(addprefix -I, $(INCL_DIR))

CC			:= gcc
CFLAGS		:= -Wall -Wextra -Werror -std=c++98

LIBS		:= #NONE

NAME		:= webserv

%.o:	%.cpp
	$(CC) $(CFLAGS) $(INCLS) -o $@ -c $<


$(NAME): $(OBJS) $(LIBS)
	#echo $(OBJS)
	$(CC) $(CFLAGS) $(INCLS) $(OBJS) $(LIBS)

all:	$(NAME)

clean:
	echo 
	#echo $(OBJS)
	rm -f $(OBJS)

fclean: clean
	#echo $(NAME)
	rm -f $(NAME)

re:		fclean all
