#include <thread>
#include <vector>
#include <math.h>
#include <iostream>
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>


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

struct firstStage: ff_node_t<int>{
    firstStage(const size_t length) : length(length){}
    int* svc(int *){
        for(size_t i = 1; i < length+1; ++i){
            ff_send_out(new int(rand()%10000+1));
        }
        return EOS;
    }
    const size_t length;
};

struct secondStage: ff_node_t<int>{
    int* svc(int* task){
        auto &t = *task;
        auto counter = 0;
        for(auto i = t; i > 1; --i)
            if(isPrime(i))
                counter++;
        t = counter;
        return task;
    }
};

struct thirdStage: ff_node_t<int>{
    int* svc(int* task){
        auto &t = *task;
        sum+=t;
        delete task;
        return GO_ON;
    }

    void svc_end(){ std::cout << "Prime numbers = " << sum << std::endl;}
    int sum = 0;
};


int main(int argc, const char** argv){
    if(argc < 2){
        std::cerr << "use: " << argv[0] << "n_tasks\n";
        return -1;
    }

    firstStage first(atoi(argv[1]));
    secondStage second;
    thirdStage third;

    ff_Pipe<int> pipe (first, second, third);
    if (pipe.run_and_wait_end() < 0){
        error("running pipe");
        return -1;
    }
    std::cout << "Time: " << pipe.ffTime() << "\n";
    //pipe.ffStats(std::cout);

    return 0;
}
