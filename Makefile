
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

SRVCORE			:= $(addprefix $(SRVCORE_DIR), $(SRVCORE_FILES))

RQST_FILES		:=	Request.cpp DataPart.cpp
RQST			:= $(addprefix $(RQST_DIR), $(RQST_FILES))

RESP_FILES		:=	Response.cpp ErrorResponse.cpp
RESP			:= $(addprefix $(RESP_DIR), $(RESP_FILES))

CGI_FILES		:=	CGIAgent.cpp
CGI				:= $(addprefix $(CGI_DIR), $(CGI_FILES))


LOGGER_FILES	:=	Logger.cpp
LOGGER			:= $(addprefix $(LOGGER_DIR), $(LOGGER_FILES))

SRC_FILES		:=	webserv_utils.cpp	\
					main.cpp
					
SRCS			:= $(addprefix $(SRC_DIR), $(SRC_FILES))


OBJS		:=	$(PARS:.cpp=.o)		\
				$(SRVCORE:.cpp=.o)	\
				$(RQST:.cpp=.o)		\
				$(RESP:.cpp=.o)		\
				$(CGI:.cpp=.o)		\
				$(LOGGER:.cpp=.o)	\
				$(SRCS:.cpp=.o)
				

INCLS		:= $(addprefix -I, $(INCL_DIRS))

CC			:= g++
CFLAGS		:= -Wall -Wextra -Werror -std=c++98 

NAME		:= webserv

%.o:	%.cpp
	$(CC) $(CFLAGS) $(INCLS) -o $@ -c $<


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLS) -o $(NAME) $(OBJS)

all:	$(NAME)


clean:
	echo 
	#echo $(OBJS)
	rm -f $(OBJS)

fclean: clean
	#echo $(NAME)
	rm -f $(NAME)

re:		fclean all
