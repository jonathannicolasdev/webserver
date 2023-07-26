
CLS_DIR		:= cls/
PARS_DIR	:= parsing/
SRC_DIR		:= src/

CLS_FILES	:= IServer.cpp			\
			   Logger.cpp			\
			   Request.cpp			\
			   AServerReactive.cpp	\
			   AServerDispatchSwitch.cpp	\
			   ServerHTTP.cpp		\
			   __EventListener.cpp	\
			   AServerCluster.cpp
#			   ServerFactory.cpp	
#			   AServerCluster.cpp	
#			   ClusterWeb.cpp

PARS_CLS		:=	ConfigBuilder.cpp	\
					LocationConfig.cpp	\
					ServerConfig.cpp	
#					main.cpp

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

PARS		:= $(addprefix $(PARS_DIR), $(PARS_CLS))

OBJS		:= $(CLS:.cpp=.o) $(SRCS:.cpp=.o) $(PARS:.cpp=.o)
ST_OBJS		:= $(ST_CLS:.cpp=.o) $(ST_SRCS:.cpp=.o)
PARS_OBJS	:= $(PARS:.cpp=.o)

CLS_INCL	:= cls/
PARS_INCL	:= parsing/
SRC_INCL	:= inc/

INCL_DIR	:= $(CLS_INCL) $(PARS_INCL) $(SRC_INCL)
INCLS		:= $(addprefix -I, $(INCL_DIR))

CC			:= g++
CFLAGS		:= -Wall -Wextra -Werror -std=c++98 -fsanitize=address

LIBS		:= #NONE

NAME		:= webserv
ST_NAME		:= simple_test_srv
PARS_NAME	:= parsing_test

%.o:	%.cpp
	$(CC) $(CFLAGS) $(INCLS) -o $@ -c $<


$(NAME): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLS) -o $(NAME) $(OBJS) $(LIBS)

all:	$(NAME)

simple_test_server: $(ST_OBJS) $(LIBS)
	#rm $(ST_NAME)
	$(CC) $(CFLAGS) $(INCLS) -o $(ST_NAME) $(ST_OBJS) $(LIBS)
	./$(ST_NAME)

parsing_test:	$(PARS_OBJS)
	$(CC) $(CFLAGS) $(INCLS) -o $(PARS_NAME) $(PARS_OBJS)
	./$(PARS_NAME)


clean:
	echo 
	#echo $(OBJS)
	rm -f $(OBJS)

fclean: clean
	#echo $(NAME)
	rm -f $(NAME)

re:		fclean all
