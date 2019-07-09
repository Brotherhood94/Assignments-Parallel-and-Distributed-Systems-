//Authors Alessandro Berti, Eugenio Paluello

#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <stdlib.h>
#include <ff/ff.hpp>

using namespace ff;


//////////////////////////////////////////////////
std::vector<int> int_generator(long size, long seed){
	srand(seed);
	std::vector<int> v(size);
	for(auto i = 0; i < size; i++)
		v[i] = rand()%23;
	return v;
}
//////////////////////////////////////////////////


template<typename T>
struct task {
	task(const long start, const long end, std::vector<T> *input_vector): start(start), end(end), input_vector(input_vector){}
	long start;
	long end;
	std::vector<T>* input_vector;
 };     

template<typename T>
struct Master: ff_monode_t<task<T>>{

	Master(std::vector<T> input_vector, long nLW): input_vector(input_vector), nLW(nLW){}
	
	task<T> *svc(task<T>*){
		long chunk = ceil(input_vector.size()/nLW), wid = 0, start = 0, end = 0;
		while(wid < nLW){
			end = (wid == nLW-1) ? input_vector.size()-1 : start+chunk-1;
			task<T> * t = new task<T>(start, end, &input_vector);
			this->ff_send_out_to(t, wid);
			start=end+1;
			wid++;
		}
		return this->EOS;
	}
	long nLW;
	std::vector<T> input_vector;
};


template<typename T>
struct MapWorker : ff_monode_t<task<T>, std::pair<T,long>>{

	MapWorker(long nRW): nRW(nRW){}

	long select_wid_reduce(T value, T module){ //cambiare qui e riga sotto
		long wid_assigned = std::hash<T>()(value)%module;
		return wid_assigned;
	}

	std::pair<T,long> *svc(task<T> *in){
		task<T> &t = *in;	
		for(auto i = t.start; i <= t.end; i++)
			hashmap[(*t.input_vector)[i]] += 1;
		delete in;	
		return this->GO_ON;
	}

	void eosnotify(ssize_t id = -1){
		for(auto& entry : hashmap){
			long dest = select_wid_reduce(entry.first, nRW);
			this->ff_send_out_to(&entry, dest);
		}
		return;
	}

	long nRW;
	std::map<T,long> hashmap;
};

template<typename T>
struct MultiInputHelper: ff_minode_t<std::pair<T,long>>{ //Necessario perchè altrimenti arrivavano sul Reduce tanti EOS quanti map thread (poichè è una a2a)

	std::pair<T,long> *svc(std::pair<T,long> *in){
		return in;
	}
	
};

template<typename T>
struct ReduceWorker : ff_minode_t<std::pair<T,long>, std::vector<std::pair<T,long>>>{

	std::vector<std::pair<T,long>> *svc(std::pair<T,long>* in){	
		std::pair<T,long> &t = *in;
		hashmap[t.first] += t.second;
		return this->GO_ON;
	}

	void eosnotify(ssize_t id = -1){
		for(auto &entry : hashmap)
			partial_out.push_back(entry);
		this->ff_send_out(&partial_out);
		return;
	}

	std::map<T,long> hashmap;
	std::vector<std::pair<T,long>> partial_out;
};

template<typename T>
struct Gatherer: ff_minode_t<std::vector<std::pair<T,long>>>{
	std::vector<std::pair<T,long>> *svc(std::vector<std::pair<T,long>> * in){
		std::vector<std::pair<T,long>> &t = *in;
		complete_output.insert(complete_output.end(), t.begin(), t.end());
		return this->GO_ON;
	}

	std::vector<std::pair<T,long>> complete_output;
};

template<typename T>
std::vector<std::pair<T,long>> GoogleMapReduce(std::vector<T> input, long nLW, long nRW){
	

	//Master
	Master<T> m(input, nLW);

	//Map - Reduce
	std::vector<ff_node*> LW;
	for(auto i = 0; i < nLW; i++)
		LW.push_back(new MapWorker<T>(nRW));

	std::vector<ff_node*> RW;
	for(auto i = 0; i < nRW; i++)
		RW.push_back(new ff_comb(new MultiInputHelper<T>(), new ReduceWorker<T>()));

	ff_a2a a2a;
	a2a.add_firstset(LW);
	a2a.add_secondset(RW);

	//Gatherer
	Gatherer<T> g;

	ffTime(START_TIME);
	ff_Pipe<> pipe(m,a2a,g);
	if(pipe.run_and_wait_end() < 0){
		error("Error");
		exit(EXIT_FAILURE);
	}
	std::cout << "Time: " << ffTime(GET_TIME) << " (ms)\n";
    	std::cout << "A2A Time: " << a2a.ffTime() << " (ms)\n";
	a2a.ffStats(std::cout);
	std::vector<std::pair<T,long>> output;
	output = g.complete_output;
	return output;
}

int main(int argc, char* argv[]){
	long nLW, nRW, size, seed;
	if(argc < 5){
		std::cout << "Usage: " << argv[0] << " nLW nRW size seed" << std::endl;
		return -1;
	}

	nLW = atol(argv[1]);
	nRW = atol(argv[2]);
	size = atol(argv[3]);
	seed = atol(argv[4]);

	std::vector<std::pair<int,long>> output = GoogleMapReduce<int>(int_generator(size,seed), nLW, nRW);
	
	for(auto &item : output)
		std::cout << "Key: " << item.first << " | " << "Value: " << item.second << std::endl;
	
	return 0;
}







