ifdef CLANG
CC = clang++
else
CC = g++
endif

CPPFLAGS = -std=c++17 -Wall -Werror -I../

ifdef CILKPLUS
CPPFLAGS += -DPARLAY_CILKPLUS -DCILK -fcilkplus
else ifdef SERIAL
CPPFLAGS += -DPARLAY_SEQUENTIAL
else
CPPFLAGS += -pthread
endif

ifdef DEBUG
CPPFLAGS += -Og -mcx16 -DDEBUG
else ifdef PERF
CPPFLAGS += -Og -mcx16 -march=native -g
else ifdef MEMCHECK
CPPFLAGS += -Og -mcx16 -DPARLAY_SEQUENTIAL
else ifdef MXMAC
CPPFLAGS += -O3 -arch x86_64
else
CPPFLAGS += -O3 -mcx16 -march=native
endif

all: serial wyllie sampling

serial: listrank.h generators.h serial.cpp
	$(CC) $(CPPFLAGS) serial.cpp -o serial

wyllie: listrank.h generators.h wyllie.cpp
	$(CC) $(CPPFLAGS) wyllie.cpp -o wyllie 

sampling: listrank.h generators.h sampling.cpp
	$(CC) $(CPPFLAGS) sampling.cpp -o sampling 

clean:
	rm serial wyllie sampling
