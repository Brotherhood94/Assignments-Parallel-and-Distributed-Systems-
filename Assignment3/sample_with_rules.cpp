#include <iostream>
#include <vector>

#include "./CImg.h"                                                                                                                                                                                              
using namespace std;
using namespace cimg_library;

// notable constants and types 
using board_t = cimg_library::CImg<unsigned char>;
const unsigned int alive = 000;
const unsigned int dead  = 255;


long mod(long a, long b){return (a%b+b)%b;}                                                                                  
                                                                                                                              
                                                                                                                              
unsigned int rules(int count, unsigned int current){                                                                         
         if(current == alive)                                                                                                 
                 return (count < 2 || count > 3) ? dead : alive;                                                              
         else if(current == dead)                                                                                             
                 return (count == 3) ? alive : dead;                                                                          
         return -1;                                                                                                           
}   


int count_neighbors(int i, int j, int n, board_t* board){                                                                    
	int counter_alive = 0;                                                                                               
	for(int x = i-1; x < i+2; x++)                                                                                       
		for(int y = j-1; y < j+2; y++)                                                                               
			if(x == i && y == j)                                                                                 
				continue;                                                                                    
			else                                                                                                 
				counter_alive += (*board)(mod(x,n), mod(y,n), 0, 0) == alive ? 1 : 0;                        
	return counter_alive;                                                                                                
}

int main(int argc, char * argv[]) {

  if(argc == 1) {
    cout << "Usage is: life n m seed iter init-no nw" << endl;
    return(0);
  }
  // get matrix dimensions from the command line
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  int N = atoi(argv[3]);
  int seed = atoi(argv[4]);

  cout << "Using " << n << "x" << m << " board " << endl;
  
  // create an empty board
  board_t board(n,m,1,1,0);

  // initialize it // randomly
  srand(seed);
  for(int i=0; i<n; i++)
    for(int j=0;  j<m; j++)
      board(i,j,0,0) = (rand() % 32 == 0 ? alive : dead);
  
  CImgDisplay main_displ(board,"Init");
  sleep(2);  // show for two seconds

  int iter = 0;
  while(iter < N) {
    // change some individuals
	  for(int i = 0; i <= n; i++)                                                                      
		  for(int j = 0; j <= m; j++)                                                              
			  board(i, j, 0, 0) = rules(count_neighbors(i, j, n, &board), board(i, j, 0, 0));
  
    // and redisplay
    main_displ.display(board);
    iter++;
  }
  return(0);
  
}
