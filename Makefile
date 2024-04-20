#To run files in terminal without make --Follow below commands
# git clone https://github.com/rashmi0305/RISCV-Simulator---Innovators.git
# cd RISCV-Simulator---Innovators
# g++ -o processor processor.cpp
# ./processor
CC = g++
CFLAGS = -std=c++11 -Wall

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = processor

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
