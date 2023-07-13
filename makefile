SRC_DIR = source
OBJ_DIR = object
INC_DIR = include
LIB_DIR = lib
BIN_DIR = binary
TESTS_DIR = tests
CC=gcc
CFLAGS= -g -Wall -I $(INC_DIR)
LIBS = -lm -lpthread -lrt -lprotobuf-c

sd: tree_client tree_server

clean: 
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*

%.o: $(SRC_DIR)/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $< 

# %.o: $(TESTS_DIR)/%.c $($@)
# 	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $< 

tree_client_OBJ = data.o entry.o tree.o client_stub.o network_client.o message.o sdmessage.pb-c.o tree_client.o
tree_client_OBJ_noTree = data.o entry.o tree.o client_stub.o network_client.o message.o sdmessage.pb-c.o tree_client.o

tree_client: $(tree_client_OBJ_noTree)
	$(CC) $(addprefix $(OBJ_DIR)/,$(tree_client_OBJ)) -o binary/$@ $(LIBS) -I $(INC_DIR)

tree_server_OBJ = data.o entry.o tree.o network_server.o message.o sdmessage.pb-c.o tree_server.o tree_skel.o
tree_server_OBJ_noTree = data.o entry.o network_server.o message.o sdmessage.pb-c.o tree_server.o tree_skel.o

tree_server: $(tree_server_OBJ_noTree)
	$(CC) $(addprefix $(OBJ_DIR)/,$(tree_server_OBJ)) -o $(BIN_DIR)/$@ $(LIBS) -I $(INC_DIR) 