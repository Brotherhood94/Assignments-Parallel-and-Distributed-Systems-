//Authors Alessandro Berti, Eugenio Paluello

#include <iostream>
#include <stdlib.h> 
#include <vector>
#include <map>
#include <thread>
#include <math.h>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>

using std::chrono::steady_clock;

//Alternative:
//
//1) Provando a fare tutto senza sincronizzazione, serve un vettore le cui posizioni sono accedute solamente da un unico thread. Ciò significa
//che avremmo tante posizioni quanti i thread (si pensi a 128 thread) molte delle quali potrebbero essere a 0, occupando quindi memoria e appesantendo l'ultima reduce.
//
//2) Nel caso volessimo sincronizzare la struttura, sempre utilizzando una map che permette di prepare il workspace per il thread reduce, dovremmo associare una
//lock a ciascuna possibile chiave (->lock tutte diverse tra loro) ed utilizzare la funzione push_back di vector dato che non sappiamo a priori quanto sarà la dimensione di ciascun vector.
//
//3) Potremmo inserire i risultati ottenuti dalla local_reduce in un unico vettore (es <p,1> <r,5> <a,4>) ma poi dovremmo (dopo il join dei map_phase thread) fare un sort e dividere il vettore
//in chunk omogenei (ovvero memorizzare start, end) ma questo implica che dovremmo scorrere l'intero vettore dei risultati e quindi, in realtà, mentre scorriamo con il for avremmo potuto
//già fare l'ultima reduce (al netto di ottimizzazioni che permetterebbo di individuare sub-array in tempo logaritmico).

template<class T> class GoogleMapReduce{
	private:
		long nw, input_size, counter_phase;
		std::vector<T> input;
		std::vector<std::pair<T,long>> output;
		std::map<long, std::map<T, std::vector<long>>> dispatcher; //map an array of homogeneous value (i.e. with the same key) to a logical workspace that will be assigned to the reduce thread denoted by wid_assigned (line 45)
		std::function<std::pair<T, long> (T)> map_function;
		std::function<std::pair<T, long> (std::pair<T, long>, std::pair<T, long>)> reduce_function;
		std::vector<std::thread*> vthreads;
		std::mutex mutex, mutex_phase;
		std::condition_variable counter_phase_cv;

		void map_phase(long start, long end, long actual_wid){
			long wid_assigned = 0;
			std::map<T,long> hashmap;
			std::pair<T,long> pair;
			for(auto i = start; i <= end; i++){
				pair = this->map_function(this->input[i]); //map	
				hashmap[pair.first] += pair.second; //local_reduce
			}
			for(auto& t : hashmap){
				std::vector<long> *res_vect = &dispatcher[wid_assigned][t.first];
				res_vect->resize(this->nw);
				wid_assigned = select_wid_reduce(t.first);
				(*res_vect)[actual_wid] = t.second;
			}
				
			return;
		}

		void reduce_phase(long actual_wid){
			std::map<T, std::vector<long>> *wid_workspace = &dispatcher[actual_wid];
			std::pair<T, long> sum, temp;
			for(auto &item : *wid_workspace){
				sum.first = item.first;
				sum.second = 0;
				for(auto i = 0; i < item.second.size(); i++){ //abbiamo taroccato la reduce, sommando sequenzialmente, dato che utilizziamo un singolo thread per eseguire la reduce. Questa scelta è una conseguenza di non aver voluto utilizzare meccanismi di sicronizzazioni. 
					temp = std::pair(item.first, item.second[i]);
					sum = this->reduce_function(sum, temp);
				}
				std::lock_guard<std::mutex> lock(mutex);
				output.push_back(sum);
			}
			return;
		}

		void thread_code(int start, int end, int actual_wid){
			auto start_time = std::chrono::high_resolution_clock::now();
			this->map_phase(start,end,actual_wid);
			auto elapsed = std::chrono::high_resolution_clock::now() - start_time;
			std::cout << "Thread: " << actual_wid << " Map Phase: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " usec" << std::endl;
			std::unique_lock<std::mutex> lck(mutex_phase); //Barrier: wait for each thread have compute the map_phase
			--counter_phase;
			counter_phase == 0 ? counter_phase_cv.notify_all() : counter_phase_cv.wait(lck, [=]{return counter_phase == 0;}); 
			start_time = std::chrono::high_resolution_clock::now();
			this->reduce_phase(actual_wid);
			elapsed = std::chrono::high_resolution_clock::now() - start_time;
			std::cout << "Thread: " << actual_wid << " Reduce Phase: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " usec" << std::endl;
			return;
		}

		long select_wid_reduce(T value){
			long wid_assigned = std::hash<T>()(value)%this->nw;
			//std::cout << "Hash: wid " << wid_assigned << " value " << value << std::endl;
			return wid_assigned;
		}

	public:
		GoogleMapReduce(long nw, 
				std::vector<T> input, 
				std::function<std::pair<T, long> (T)> map_function,
				std::function<std::pair<T, long> (std::pair<T, long>, std::pair<T, long>)> reduce_function){
			this->nw = this->counter_phase = nw;
			this->input = input;
			this->input_size = this->input.size();
			this->map_function = map_function;
			this->reduce_function = reduce_function;
			this->vthreads = std::vector<std::thread*>(nw);
		}


		std::vector<std::pair<T,long>> run(){ 
			long chunk = ceil(this->input_size/this->nw); 
			long counter = 0, start = 0, end = 0;
			while(counter < this->nw){
				end = (counter == this->nw-1) ? this->input_size-1 : start+chunk-1;
				vthreads[counter] = new std::thread([=]{thread_code(start, end, counter);});
				start=end+1;
				counter++;
			}
			for(auto i = 0; i < vthreads.size(); i++)
				vthreads[i]->join();

			return output;
		}
};


