//@Authors Alessandro Berti, Luca Corbucci, Eugenio Paluello

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <math.h>
#include <chrono>
#include "./CImg.h"                                                                                                    

using namespace std;
using namespace cimg_library;
using std::chrono::steady_clock;

// notable constants and types 
using board_t = cimg_library::CImg<unsigned char>;
const unsigned int alive = 255;
const unsigned int dead  = 000;

//synchronization variable
int counter = 0;
std::mutex d_mutex;
std::condition_variable s_condition;        //rectangle 
std::condition_variable b_condition;        //board	

//times variable
auto fitness_update_time = steady_clock::duration::zero();
auto display_time = steady_clock::duration::zero();


struct rectangle{int x1, y1, x2, y2;};

long mod(long a, long b){return (a%b+b)%b;}

int find_div(int val, int sq){
	while(sq%val!=0)
		val++;
	return val;
}

unsigned int rules(int count, unsigned int current){
	if(current == alive)
		return (count < 2 || count > 3) ? dead : alive;
	else if(current == dead)
		return (count == 3) ? alive : dead;
	return -1;
}

void pattern(board_t* board, int n){
	for(int i=0; i<n; i++)
		for(int j=0; j<n; j++){
			if(j > n/2-10 && j < n/2+10)
				(*board)(i,j,0,0) = alive;
			if(i > n/2-10 && i < n/2+10)
				(*board)(i,j,0,0) = alive;
		}
	return;
}

void random(board_t* board, int n){
	for(int i=0; i<n; i++)
		for(int j=0;  j<n; j++)
			(*board)(i,j,0,0) = (rand() % 32 == 0 ? alive : dead);
	return;
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


void update_rectangle(board_t *new_board, board_t* old_board, rectangle rect, int N, int n){
	int iter = 0;
	while(iter < N){
		auto start = chrono::high_resolution_clock::now();
		for(int i = rect.x1; i <= rect.x2; i++)
			for(int j = rect.y1; j <= rect.y2; j++)
					(*new_board)(i, j, 0, 0) = rules(count_neighbors(i, j, n, old_board), (*old_board)(i, j, 0, 0));
		iter++;
		std::unique_lock<std::mutex> lock(d_mutex);
		fitness_update_time += chrono::high_resolution_clock::now() - start;
		counter++;
		b_condition.notify_one();
		s_condition.wait(lock);
	}
	return;
}

void render(board_t *new_board, board_t* old_board, CImgDisplay* main_displ, int N, int nw){
	board_t temp;
	int iter = 0;
	while(iter < N){
		std::unique_lock<std::mutex> lock(d_mutex);
		b_condition.wait(lock, [=]{return counter == nw;});
		auto start = chrono::high_resolution_clock::now();
		(*main_displ).display(*new_board);
		temp = *old_board;
		*old_board = *new_board;
		*new_board = temp;
		display_time += chrono::high_resolution_clock::now() - start;
		counter = 0;
		s_condition.notify_all();
		iter++;
	}
	return; 	
}

void board_partitions(std::vector<rectangle>* rectangles, int n, int nw){ 
	int x_size, y_size, tot_size = n*n/nw;
	y_size = tot_size / (x_size = find_div(ceil(sqrt(tot_size)), tot_size));
	int per_rows = n/x_size;
	int r = (n-x_size*per_rows)/per_rows;
	int rr = (n-x_size*per_rows)-(r*per_rows); 
	int x1=0, y1=0, x2=x_size-1+r+rr, y2=y_size-1;
	int count_row = 1;
	for(int wid = 0; wid < nw; wid++){	
		(*rectangles)[wid].x1 = x1; 
		(*rectangles)[wid].y1 = y1; 
		(*rectangles)[wid].x2 = x2; 
		(*rectangles)[wid].y2 = y2;
		if(count_row < per_rows){
			x1 += x_size;
			x2 += x_size+r;
			count_row++;	
		}
		else{
			x1 = 0; x2 = x_size-1+r+rr;
			y1 += (y1 + y_size < n) ? y_size : n-y1-1;
			y2 += (y2 + y_size < n) ? y_size : n-y2-1;
			count_row = 1;
		}
	}
	return;
}

int main(int argc, char * argv[]) {
	if(argc < 5) {
		cout << "Usage is: " << argv[0] << " board_size iters nworkers random.\nEx: "<< argv[0] <<" 512 1000 8 0" << endl;
		return(0);
	}
	int n = atoi(argv[1]);
	int N = atoi(argv[2]);
	int nw = atoi(argv[3]);
	int random_init = atoi(argv[4]);

	// create empty boards
	board_t old_board(n,n,1,1,0);
	board_t new_board(n,n,1,1,0);
	
	//vector of threads
	std::vector<std::thread> threads(nw);

	//vector of rectangles 
	std::vector<rectangle> rectangles(nw);

	//Assign a certain area to each thread 
	board_partitions(&rectangles, n, nw);

	//Initialization
	random_init ? random(&old_board, n) : pattern(&old_board, n); 
	CImgDisplay main_displ(old_board,"Game of Life");

	auto start = chrono::high_resolution_clock::now();
	std::thread renderer = std::thread(render, &new_board, &old_board, &main_displ, N, nw); 
	for(int i = 0; i < nw; i++)
		threads[i] = std::thread(update_rectangle, &new_board, &old_board, rectangles[i], N, n);

	for(int i = 0; i < nw; i++)
		threads[i].join();
	renderer.join();
	auto elapsed = chrono::high_resolution_clock::now() - start;

	cout << "Per Thread Average Fitness_Update Time: "<< chrono::duration_cast<chrono::microseconds>(fitness_update_time).count()/n/N << " usec" << endl;
	cout << "Average Display Time: "<< chrono::duration_cast<chrono::microseconds>(display_time).count()/N << " usec" << endl;
	cout << "Whole Time: "<< chrono::duration_cast<chrono::microseconds>(elapsed).count() << " usec" << endl;

	return(0);
}
