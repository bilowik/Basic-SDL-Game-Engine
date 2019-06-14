# Global Variables
export MYLIBS=/home/aflores/programming/mylibs
export G_PROJECT_NAME=simpleGameEngine
export G_SRC_DIR=$(shell pwd)/src
export G_REL_CURR_DIR=# keeps track of the current directory relative to the src directory
export G_PRJCFLAGS= -Wall 
export G_OBJ_FILES= # this is where the paths to all the obj files go after they are created
export G_OBJ_DIR=$(shell pwd)/bin/obj
export G_LDFLAGS=-Wl,-rpath='$${ORIGIN}'
export G_LIBS= -L$(BIN) -L$(MYLIBS) -lvector -lSDL2 -lSDL2_image -lkeymap \
-lmap -lSDL2_ttf -lqueue_int -lmap_f -lmapi -lm
export G_CFLAGS=$(EXTRA_CFLAGS) -Wall -I$(MYLIBS) -I$(G_SRC_DIR)
export CC=gcc

# Source and header variables

HEADER_DIR=headers
SRC_DIR=src

CLIENT_DIR=$(SRC_DIR)/clients
CLIENT_FILES := $(wildcard $(CLIENT_DIR)/*.c)
CLIENTS=$(CLIENT_FILES:$(CLIENT_DIR)/%.c=%)

# Object and binary variables
BIN=bin
OBJ_DIR=$(BIN)/obj
CLIENT_OBJ_DIR=$(OBJ_DIR)/clients
CLIENT_OBJ_FILES=$(CLIENT_FILES:$(CLIENT_DIR)/%.c=$(CLIENT_OBJ_DIR)/%.o)
CLIENT_EXES=$(CLIENTS:%=$(BIN)/%)
# Recipe Variables 
CC=gcc
AS=as
LIBS=$(G_LIBS)
CFLAGS=$(G_CFLAGS)
LDFLAGS=$(G_LDFLAGS)


#set vpath
VPATH=$(G_SRC_DIR)

# Set debug flags for all compilation
debug-all: G_CFLAGS += -g -O0
debug-all: all

all: pre-build
	$(MAKE) -C $(G_SRC_DIR)
#debug-engine: G_CFLAGS += -g -O0
#debug-engine: engine
#engine: pre-build
#	$(MAKE) -C $(G_SRC_DIR)/engine

#debug-game: G_CFLAGS += -g -O0
#debug-game: game
#game: pre-build
#	$(MAKE) -C $(G_SRC_DIR)/game
	
pre-build:
	-rm $(G_SRC_DIR)/.obj_files.tmp


debug-test: G_CFLAGS += -g -O0
debug-test: test
test: $(CLIENT_OBJ_DIR)/testclient.o all
	$(eval G_OBJ_FILES := $(shell cat $(G_SRC_DIR)/.obj_files.tmp))
	$(CC) $(LDFLAGS) $(LIBS) -o $(BIN)/$@ $(G_OBJ_FILES) $(wildcard bin/obj/game/test/*.o) $<

debug-osGame: G_CFLAGS += -g -O0
debug-osGame: osGame
osGame: $(CLIENT_OBJ_DIR)/osGame.o all
	$(eval G_OBJ_FILES := $(shell cat $(G_SRC_DIR)/.obj_files.tmp))
	$(CC) $(LDFLAGS) $(LIBS) -o $(BIN)/$@ $(G_OBJ_FILES) $(wildcard bin/obj/game/osGame/*.o) $<

debug-filePathHandlerTest : CFLAGS += -g
debug-filePathHandlerTest : filePathHandlerTest
filePathHandlerTest : $(OBJ_DIR)/filePathHandler.o $(CLIENT_OBJ_DIR)/filePathHandlerTest.o 
	$(CC) $(LDFLAGS) $(LIBS) -o $(BIN)/$@ $^ 

debug-inputHandlerTest : CFLAGS += -g
#debug-inputHandlerTest : LDFLAGS += -g
debug-inputHandlerTest : inputHandlerTest
inputHandlerTest : $(OBJ_DIR)/inputHandler.o $(CLIENT_OBJ_DIR)/inputHandlerTest.o
	$(CC) $(LDFLAGS) $(LIBS) -o $(BIN)/$@ $^

build-sprite-db:
	$(CC) build-sprite-db.c -o bin/build-sprite-db
	bin/build-sprite-db

# Client object build rules
$(CLIENT_OBJ_FILES) : $(CLIENT_OBJ_DIR)/%.o : $(CLIENT_DIR)/%.c
		$(CC) -c $(CFLAGS) -o $@ $<

# C source build rules
$(C_OBJ_FILES) : $(OBJ_DIR)/%.o : %.c %.h
		$(CC) -c $(CFLAGS) -o $@ $<

# Assembly source build rules
$(AS_OBJ_FILES) : $(OBJ_DIR)/%.o : %.s
		$(AS) -o $@ $<

# Cleaning
.PHONY : clean

clean :
	$(MAKE) -C $(G_SRC_DIR) clean
	rm -rf $(CLIENT_EXES) $(OBJ_DIR)/build-item-db.o $(CLIENT_OBJ_FILES)
# Print rule for debuggin
#	To print out a variable:
#		$ make print-VARIABLE
print-% : ; @echo $* = $($*)




