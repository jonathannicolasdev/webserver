
CLS_DIR		:= cls/
SRC_DIR		:= src/

CLS_FILES	:= IServer.cpp			\
			   Logger.cpp			\
			   Request.cpp			\
			   AServerReactive.cpp	\
			   AServerDispatchSwitch.cpp	\
			   ServerHTTP.cpp		
#			   ServerFactory.cpp	
#			   AServerCluster.cpp	
#			   ClusterWeb.cpp

ST_CLS_FILES	:= IServer.cpp			\
				   AServerReactive.cpp	\
				   ServerHTTP.cpp		\
				   Logger.cpp

SRC_FILES		:= webserv_utils.cpp #main.cpp

ST_SRC_FILES	:= simple_server_test.cpp	\
				   signal_handlers.cpp		\
				   webserv_utils.cpp		
		   

CLS			:= $(addprefix $(CLS_DIR), $(CLS_FILES))
SRCS		:= $(addprefix $(SRC_DIR), $(SRC_FILES))
ST_CLS		:= $(addprefix $(CLS_DIR), $(ST_CLS_FILES))
ST_SRCS		:= $(addprefix $(SRC_DIR), $(ST_SRC_FILES))

OBJS		:= $(CLS:.cpp=.o) $(SRCS:.cpp=.o)
ST_OBJS		:= $(ST_CLS:.cpp=.o) $(ST_SRCS:.cpp=.o)

CLS_INCL	:= cls/
SRC_INCL	:= inc/

INCL_DIR	:= $(CLS_INCL) $(SRC_INCL)
INCLS		:= $(addprefix -I, $(INCL_DIR))

CC			:= g++
CFLAGS		:= -Wall -Wextra -Werror -std=c++98

LIBS		:= #NONE

NAME		:= webserv
ST_NAME		:= simple_test_srv

%.o:	%.cpp
	$(CC) $(CFLAGS) $(INCLS) -o $@ -c $<


$(NAME): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLS) -o $(NAME) $(OBJS) $(LIBS)

all:	$(NAME)

simple_test_server: $(ST_OBJS) $(LIBS)
	#rm $(ST_NAME)
	$(CC) $(CFLAGS) $(INCLS) -o $(ST_NAME) $(ST_OBJS) $(LIBS)
	./$(ST_NAME)

clean:
	echo 
	#echo $(OBJS)
	rm -f $(OBJS)

fclean: clean
	#echo $(NAME)
	rm -f $(NAME)

re:		fclean all
