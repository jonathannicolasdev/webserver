
PARS_DIR		:= Parsing/
SRVFACT_DIR		:= ServerFactory/
SRVCORE_DIR		:= ServerCore/
RQST_DIR		:= Request/
RESP_DIR		:= Response/
CGI_DIR			:= CGI/
LOGGER_DIR		:= Logger/
SRC_DIR			:= src/

INCL_DIRS		:=	inc/ $(PARS_DIR) $(SRVFACT_DIR) $(SRVCORE_DIR) \
					$(RQST_DIR) $(RESP_DIR) $(LOGGER_DIR) $(SRC_DIR)\
					$(CGI_DIR)


PARS_FILES		:=	ConfigBuilder.cpp	\
					LocationConfig.cpp	\
					ServerConfig.cpp	
#					main.cpp
PARS			:= $(addprefix $(PARS_DIR), $(PARS_FILES))

SRVFACT_FILES	:=	ServerFactory.cpp
SRVFACT			:= $(addprefix $(SRVFACT_DIR), $(SRVFACT_FILES))

SRVCORE_FILES	:= IServer.cpp					\
				   AServerReactive.cpp			\
				   AServerDispatchSwitch.cpp	\
				   ServerHTTP.cpp				\
				   __EventListener.cpp			\
				   AServerCluster.cpp			\
				   ServerFactory.cpp	
#			   AServerCluster.cpp
#			   ClusterWeb.cpp

SRVCORE			:= $(addprefix $(SRVCORE_DIR), $(SRVCORE_FILES))

RQST_FILES		:=	Request.cpp
RQST			:= $(addprefix $(RQST_DIR), $(RQST_FILES))

RESP_FILES		:=	Response.cpp ErrorResponse.cpp
RESP			:= $(addprefix $(RESP_DIR), $(RESP_FILES))

CGI_FILES		:=	CGIAgent.cpp
CGI				:= $(addprefix $(CGI_DIR), $(CGI_FILES))


LOGGER_FILES	:=	Logger.cpp
LOGGER			:= $(addprefix $(LOGGER_DIR), $(LOGGER_FILES))

SRC_FILES		:=	webserv_utils.cpp	\
					main.cpp
#					signal_handlers.cpp
					
SRCS			:= $(addprefix $(SRC_DIR), $(SRC_FILES))


ST_CLS_FILES	:= IServer.cpp			\
				   AServerReactive.cpp	\
				   ServerHTTP.cpp		\
				   Logger.cpp

#SRC_FILES		:= webserv_utils.cpp #main.cpp

ST_SRC_FILES	:= simple_server_test.cpp	\
				   signal_handlers.cpp		\
				   webserv_utils.cpp		
		   

#CLS			:= $(addprefix $(CLS_DIR), $(CLS_FILES))
#SRCS		:= $(addprefix $(SRC_DIR), $(SRC_FILES))
#ST_CLS		:= $(addprefix $(CLS_DIR), $(ST_CLS_FILES))
#ST_SRCS		:= $(addprefix $(SRC_DIR), $(ST_SRC_FILES))

#PARS		:= $(addprefix $(PARS_DIR), $(PARS_CLS))

OBJS		:=	$(PARS:.cpp=.o)		\
				$(SRVCORE:.cpp=.o)	\
				$(RQST:.cpp=.o)		\
				$(RESP:.cpp=.o)		\
				$(CGI:.cpp=.o)		\
				$(LOGGER:.cpp=.o)	\
				$(SRCS:.cpp=.o)
#				$(SRVFACT:.cpp=.o)
				
#$(CLS:.cpp=.o) $(SRCS:.cpp=.o) $(PARS:.cpp=.o)


ST_OBJS		:= $(ST_CLS:.cpp=.o) $(ST_SRCS:.cpp=.o)
PARS_OBJS	:= $(PARS:.cpp=.o)

#INCL_DIR	:= $(_INCL) $(PARS_INCL) $(SRC_INCL)
INCLS		:= $(addprefix -I, $(INCL_DIRS))

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
