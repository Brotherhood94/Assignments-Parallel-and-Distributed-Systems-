CFLAGS = -pthread -std=c++17 -O3 
CC = g++

all:	
	$(CC) main.cpp $(CFLAGS) -o pthread_google_map_reduce 
	@echo "----> Ex. ./pthread_google_map_reduce 4 10000 1" 
	$(CC) fastflow_google_map_reduce.cpp $(CFLAGS) -DTRACE_FASTFLOW -o fastflow_google_map_reduce 
	@echo "----> Ex. ./fastflow_google_map_reduce 4 4 10000 1" 

clean: 
	rm -f pthread_google_map_reduce
	rm -f fastflow_google_map_reduce
	@echo "Clean done"
