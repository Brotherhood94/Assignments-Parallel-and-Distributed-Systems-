#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<unistd.h>
#include "./safe_queue.h"
#include "./pipeline_functions.h"


typedef void (*FunctionPointer) (SafeQueue<int>*,SafeQueue<int>*);


std::vector<FunctionPointer> functions {
        streamIncrease, 
        streamSquare, 
        streamDecrease,
    };


void print_threadCPU(int i, std::mutex* iomutex){
    std::lock_guard<std::mutex> lock(*iomutex);
    std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << "\n";
    return;
}


void body(int i, int m, std::vector<FunctionPointer> functions, std::mutex* iomutex, std::vector<SafeQueue<int>*>* safeQueues){
    print_threadCPU(i, iomutex);
    if(i == 0)
        streamInt(m, safeQueues->at(i));
    else if( i > 0 && i <= functions.size())
        functions[i - 1](safeQueues->at(i-1), safeQueues->at(i));
    else
        printAll(safeQueues->at(i-1));
    return;    
}

int main(int argc, const char** argv) {
    int num_threads = 5;
    int m = atoi(argv[1]);
    int cond = atoi(argv[2]);

    std::mutex iomutex;     // A mutex ensures orderly access to std::cout from multiple threads.
    std::vector<std::thread> threads(num_threads);

    std::vector<SafeQueue<int>*> safeQueues;
    safeQueues.resize(0);
    for(int i = 0; i < num_threads-1; i++)
        safeQueues.push_back(new SafeQueue<int>());

    (cond) ? std::cout << "Sticky" << std::endl : std::cout << "Not Sticky" << std::endl;
    for (int i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(body, i, m, functions, &iomutex, &safeQueues);
        if(cond){
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i, &cpuset);
            int error = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
            if (error != 0)
                std::cerr << "Error calling pthread_setaffinity_np: " << error << "\n";
        }
    }

    for (auto& t : threads)
        t.join();

    return 0;
}
