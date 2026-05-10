# Custom Variables

CXX = g++

CXXFLAGS = -Wall -Wextra -O2

# Only need to include .o files because there's a rule defined for them at the end of the file
# (except like .h files that dont have .cpp counterpart or .cpp files that dont have .cpp like main.cpp)
COMMON_OBJS = src/common/constants.h src/common/operations.o
SERVER_OBJS = $(COMMON_OBJS) src/server/main.cpp src/server/server_support.o
CLIENT_OBJS = $(COMMON_OBJS) src/client/main.cpp


# Make rules

# Default targets, running make will build these 2 binaries (in these paths)
all: ./ES ./user


# Rule for building server binary (specify all .o files (object files) that should be involved) and the compiling command to execute
./ES: $(SERVER_OBJS)
	$(CXX) $(CFLAGS) -o $@ $(SERVER_OBJS) 


# Rule for building client binary (specify all .o files (object files) that should be involved) and the compiling command to execute
./user: $(CLIENT_OBJS)
	$(CXX) $(CFLAGS) -o $@ $(CLIENT_OBJS) 


# States each object file (.o) depends on both the .cpp and .h parts and the compiling command to build the object file
%.o: %.cpp %.h
	$(CXX) $(CFLAGS) -c ${@:.o=.cpp} -o $@


clean:
	rm -f src/common/*.o src/server/*.o ./ES src/client/*.o ./user




