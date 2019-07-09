
#include <iostream>
#include <vector>
#include <functional>
#include <experimental/optional>
#include <grppi.h>

using namespace std;
using namespace std::literals::chrono_literals;

#include "utimer.hpp"

void drain(int x) {
      this_thread::sleep_for(10ms);
      cout << x << endl;
}  
int main(int argc, char * argv[]) {

  auto m = atoi(argv[1]);
  auto nw = atoi(argv[2]); 
  
  grppi::dynamic_execution seq = grppi::sequential_execution{};
  grppi::dynamic_execution thr = grppi::parallel_execution_native{4};
  grppi::dynamic_execution omp = grppi::parallel_execution_omp{};
  {
    utimer t("Pipeline execution");

    auto s1 = [](int x) { this_thread::sleep_for(50ms); return ++x; };
    auto s2 = [](int x) { return x*x; };

		grppi::pipeline( // execution engine: native thread backend
				thr,
				/*  stream generator stage
						In questo caso partiamo dal valore m e poi andiamo verso 0
						Viene emesso uno stream di vettori 
				*/
				[m]() -> experimental::optional<vector<int> *> { 
		      static int x = m;
		      this_thread::sleep_for(10ms);
		      if (x>0)
					{
						auto v =  new vector<int>(x);
						for(int i=0; i<x; i++)
							(*v)[i] = i;
						x--;
						return v;
		      } 
					else
						return {};
				},
				/*
					Il secondo stage della pipeline prende un vettore e poi
					per ogni valore della pipeline esegue la funzione s2.
					All'interno di questo stage utilizziamo una map per applicare ad 
					ogni elemento del vettore la funzione s2.
					La map la definisco passando come parametro i puntatore all'inizio e alla
					fine del vettore e poi il puntatore al vettore dove devo salvare le modifiche.
					Poi passo la funzione s2 che deve essere eseguita.
					Nella map con grppi::parallel_execution_native{nw} gli indico quanti thread voglio
					per eseguire la map.
				*/
				[s2, nw](vector<int> *v) {
		      grppi::map(grppi::parallel_execution_native{nw},
				 begin(*v), end(*v), begin(*v),
				 s2);
		      return v;
				},
				// stream collapser stage
				[](vector<int> *v) {
		      for(auto &i : (*v))
						cout << i << " ";
		      cout << endl;
				}
				// end of the pipeline parameters
		);
	}
  
  return(0);
}
