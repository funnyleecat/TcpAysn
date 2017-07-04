ifeq	($(release),1)

FLG := -O3 -fPIC

else

FLG := -O0 -g -fPIC

endif

PRG := bin/libtcpcomm2.so

SRC := $(wildcard src/*.cpp)

OBJ := $(patsubst %.cpp,%.o,$(SRC))

CC := g++

INC :=  -I./inc -I./include -I./tcpcomm2 \
		-I../lib/cross/utils/inc \
		-I../../../lib/cross/boost/include

LIB :=  -lpthread -lrt \
		-L/usr/local/lib/boost/a -lboost_system -lboost_date_time -lboost_thread  

.PHONY: clean

$(PRG): $(OBJ)
	$(CC) -shared -o $@ $^ $(LIB)

%.o: %.cpp
	$(CC) $(FLG) -o $@ -c $< $(INC)

clean:
	rm -f $(OBJ) $(PRG)
