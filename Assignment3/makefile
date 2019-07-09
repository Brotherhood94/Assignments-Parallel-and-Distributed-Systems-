OBJECTS = gameoflife.cpp
CFLAGS = -pthread -std=c++17 -lX11
CC = g++
PROGRAM_NAME = gameoflife 

$(PROGRAM_NAME):$(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(PROGRAM_NAME)
	@echo "----> Ex. "$(PROGRAM_NAME)"512 10000 8 0" 

clean: 
	rm -f $(PROGRAM_NAME)
	rm -f $(TESTNAME)
	@echo "Clean done"
