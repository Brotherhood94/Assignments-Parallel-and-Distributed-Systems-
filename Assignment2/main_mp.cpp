#include <thread>
#include <vector>
#include <queue>
#include <iostream>
#include <math.h>
#include <omp.h>

#define EOS -1

std::queue<int>  x;    // tasks
std::queue<int>  y;    

bool isPrime(int x){
    if(x == 2)
        return true;
    if(x % 2 == 0 || x == 1){
        return false;}

    int i = 2;
    int sqR =sqrt(x);

    while(i <= sqR){
        if(x % i == 0){
            return false;
        }
        i++;
    }
    return true;
}

int main(int argc, const char** argv){
    int m = atoi(argv[1]);
    int nw = (int) std::thread::hardware_concurrency();
    for(int i = 0; i < m; ++i){
        int ran = rand()%10000+1;
        x.push(ran);
    }
    x.push(-1);
    int ex, sum;
    while( (ex = x.front()) != EOS){
        x.pop();
        sum = 0;
        for(int i = 2; i <= ex; i++){
            if(isPrime(i))
                sum++;
        }
        y.push(sum);
    }

/*
    while( (ex = y.front()) != EOS){
        y.pop();
        std::cout << ex << std::endl;
    }
*/

    return 0;
}