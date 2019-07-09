//@Author Alessandro Berti, Eugenio Paluello
#include <thread>
#include <vector>
#include <math.h>
#include <iostream>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>


using namespace ff;

int isPrime(int x){
    if(x == 2)
        return 1;
    if(x % 2 == 0)
        return 0;
    int i = 2;
    int sqR = sqrt(x);
    while(i <= sqR){
        if(x % i == 0)
            return 0;
        i++;
    }
    return 1;
}



int main(int argc, const char** argv){
    if(argc < 3){
        std::cerr << "use: " << argv[0] << " nworkers n_tasks\n";
        return -1;
    }
    int n_tasks = atoi(argv[2]);
    int threads = atoi(argv[1]);

    std::vector<int> tasks(n_tasks);
    for(int i = 0; i < n_tasks; i++)
        tasks[i] = i+2;
    
    std::vector<int> res(n_tasks,0);
    
    ParallelFor pf(threads);
    ffTime(START_TIME);
    pf.parallel_for(0, n_tasks, 1, 0, [&tasks,&res](const int i){
            for(int k = 2; k <= tasks[i]; k++)
                res[i] += isPrime(k);
        }
    ,threads);
    ffTime(STOP_TIME);
    std::cout << "Time: " << ffTime(GET_TIME) << "\n";
    //for(int i = 0; i < n_tasks; i++)
    //	std::cout << res[i] << std::endl;
    return 0;
}
