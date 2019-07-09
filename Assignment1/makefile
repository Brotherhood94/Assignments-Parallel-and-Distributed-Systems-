OBJECTS = main.cpp
CFLAGS = -std=c++11 -pthread
CC = g++
PROGRAM_NAME = main

$(PROGRAM_NAME):$(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGRAM_NAME) $(OBJECTS)
	@echo " "
	@echo "@Alessandro Berti & @Eugenio Paluello"
	@echo " "
	@echo "Compilation Completed!"
	@echo "Run with ./"$(PROGRAM_NAME)" m s"
	@echo "m = number of items to generate"
	@echo "s = 1 --> sticky thread"
	@echo "s = 0 --> not sticky thread"
	@echo "  "
	@echo " --- Not Sticky"
	@echo "GOAL 1: with m = 1000. Real time = 10.15 "
	@echo "GOAL 2: Overhead = 0.15"
	@echo "  "
	@echo " --- Sticky"
	@echo "GOAL 1: with m = 1000. Real time = 10.14 "
	@echo "GOAL 2: Overhead = 0.14"
clean: 
	rm -f $(PROGRAM_NAME)
	@echo "Clean done"




	