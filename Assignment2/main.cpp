#include <thread>
#include <vector>
#include "safe_queue.h"
#include <iostream>
#include <math.h>
#include <chrono>
#define EOS -1

typedef struct{
    int start;
    int end;
} params;


SafeQueue<int> producerQueue;
SafeQueue<int> outputQueue;
std::vector<SafeQueue<params*>>* paramsVector;
std::vector<SafeQueue<int>>* partialResult;

void producer(int numTask){
    for(int i=0;i<numTask;i++)
        producerQueue.safe_push(rand()%10000);
    producerQueue.safe_push(EOS);
    return;
}

bool isPrime(int x){
    if(x == 2)
        return true;
    if(x % 2 == 0){
        return false;}
    int i = 2;
    int sqR = sqrt(x);
    while(i <= sqR){
        if(x % i == 0)
            return false;
        i++;
    }
    return true;
}

void computePrime(int wid){
    params* p;
    int sum = 0;
    while( ( p = (*paramsVector)[wid].safe_pop())->start != EOS){
        sum = 0;
        for(int i=p->start; i<=p->end; i++)
            if(isPrime(i))
                sum++;
        (*partialResult)[wid].safe_push(sum);
    }
    return;
}

void reducer(int nw){
    params* p;
    float binLen;
    int extracted = 0, sum = 0, numBin = 0, start = 0, end = 0;
    std::vector<std::thread> threads(nw);
    for(int wid = 0; wid < threads.size(); wid++)
            threads[wid] = std::thread(computePrime, wid);

    while ((extracted = producerQueue.safe_pop()) != EOS){
        sum = 0;
        start = 2;
        binLen = (float) (extracted-start)/nw;  //-2 cuz numbers 0,1 are excluded [2,x]
        for(int wid = 0; wid < nw; wid++){  //da invertire la run --
            if(ceil(binLen) != float(binLen) && wid == nw-1)
                end =  extracted;
            else
                end =  start+(int) binLen-1;
            p = new params{start, end};
            start = start+binLen;
            (*paramsVector)[wid].safe_push(p);
        }

        for (int wid = 0; wid < nw; wid++)
            sum+=(*partialResult)[wid].safe_pop();
        outputQueue.safe_push(sum);
    }
    for (int wid = 0; wid < paramsVector->size(); wid++){
        p = new params{-1, -1};
        (*paramsVector)[wid].safe_push(p);
    }
    for (int wid = 0; wid < threads.size(); wid++){
        threads[wid].join();
    }
    return;
}

int main(int argc, const char ** argv){
    if(argc > 1){
        int numTask = atoi(argv[1]);
        int nw = atoi(argv[2]);
        paramsVector = new std::vector<SafeQueue<params*>>(nw);
        partialResult = new std::vector<SafeQueue<int>>(nw);
        std::thread producerThread = std::thread(producer, numTask);
        reducer(nw);
        producerThread.join();  
    }
    else
        std::cout << "Missing argument: numTasks" << std::endl;
    return 0;
}