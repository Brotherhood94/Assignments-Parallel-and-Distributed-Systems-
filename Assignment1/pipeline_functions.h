#include<chrono>

#define EOS -1


void streamInt(int m, SafeQueue<int>* toQueue){
    for(int i = 0; i < m; i++){
        //safeQueues.at(0)->safe_push()(i);
        toQueue->safe_push(i);
    }
    toQueue->safe_push(EOS);
    return;
}

void streamIncrease(SafeQueue<int>* fromQueue, SafeQueue<int>* toQueue){
    int v;
    while( (v = fromQueue->safe_pop()) != EOS){
        toQueue->safe_push(++v);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    toQueue->safe_push(EOS);
    return; 
}

void streamSquare(SafeQueue<int>* fromQueue, SafeQueue<int>* toQueue){
    int v;
    while( (v = fromQueue->safe_pop()) != EOS){
        toQueue->safe_push(v*v);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    toQueue->safe_push(EOS);
    return;
}

void streamDecrease(SafeQueue<int>* fromQueue, SafeQueue<int>* toQueue){
    int v;
    while((v = fromQueue->safe_pop()) != EOS){
        toQueue->safe_push(--v);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    toQueue->safe_push(EOS);
    return;
}

void printAll(SafeQueue<int>* fromQueue){
    int v;
    while ((v = fromQueue->safe_pop()) != EOS)
        std::cout << v << " ";
    std::cout << std::endl;
    return;
}