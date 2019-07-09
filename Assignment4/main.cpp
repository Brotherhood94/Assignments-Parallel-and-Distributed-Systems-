//Authors Alessandro Berti, Eugenio, Paluello

#include <stdlib.h>
#include "./pthread_google_map_reduce.h"
/////////////////////////////////////////////////////////////////////
template<typename T>
std::pair<T,long> map(T item){
	return std::make_pair(item, 1);
}

template<typename T>
std::pair<T,long> reduce(std::pair<T, long> x, std::pair<T, long> y){
	return std::make_pair(x.first, x.second+y.second);
}
/////////////////////////////////////////////////////////////////////

//
/////////////////////////////////////////////
//Random_string() Taken from: https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
std::string random_string( size_t length ){
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[ rand() % max_index ];
	};
	std::string str(length,0);
	std::generate_n( str.begin(), length, randchar );
	return str;
}
/////////////////////////////////////////////


/////////////////////////////////////////////
std::vector<int> int_generator(long size, long seed){
	srand(seed);
	std::vector<int> v(size);
	for(auto i = 0; i < size; i++)
		v[i] = rand()%23;
	return v;
}
/////////////////////////////////////////////

std::vector<std::string> string_generator(int size){
	std::vector<std::string> v(size);
	for(auto i = 0; i < size; i++)
		v[i] = random_string(5);
	return v;
}
/////////////////////////////////////////////

int main(int argc, char**argv){
	long nw, size, seed;
	std::vector<std::pair<std::string, long>> string_out;
	std::vector<std::pair<int, long>> int_out;
	if(argc < 4){
		std::cerr << "Usage: " << argv[0] << " nw size" << std::endl;
		return 1;
	}
	nw = atol(argv[1]);
	size = atol(argv[2]);
	seed = atol(argv[3]);

	//Integer vector
	std::function<std::pair<int, long> (int)> map_function = map<int>;
	std::function<std::pair<int, long> (std::pair<int, long>, std::pair<int, long>)> reduce_function = reduce<int>;
	std::cout << "------------ Start passing by copy input vector to GoogleMapReduce object" << std::endl;
	GoogleMapReduce<int> gmr(nw, int_generator(size, seed), map_function, reduce_function); 
	std::cout << "------------ End passing by copy input vector to GoogleMapReduce object" << std::endl;
	int_out = gmr.run();	
	for(auto &item : int_out)
		std::cout << "Key: " << item.first << "--- Value: " << item.second << std::endl;
	
	//String vector	
//	std::function<std::pair<std::string, long> (std::string)> map_function = map<std::string>;
//	std::function<std::pair<std::string, long> (std::pair<std::string, long>, std::pair<std::string, long>)> reduce_function = reduce<std::string>;
//	std::cout << "------------ Start passing by copy input vector to GoogleMapReduce object" << std::endl;
//	GoogleMapReduce<std::string> gmr(nw, string_generator(size), map_function, reduce_function); 
//	std::cout << "------------ End passing by copy input vector to GoogleMapReduce object" << std::endl;
//	string_out = gmr.run();
//	
//	for(auto &item : string_out)
//		std::cout << "Key: " << item.first << "--- Value: " << item.second << std::endl;
//
	return 0;
}
