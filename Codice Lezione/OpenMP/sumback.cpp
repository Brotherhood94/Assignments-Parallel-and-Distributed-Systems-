#include <iostream>
#include <omp.h>
#include <vector>

int main(int argc, char *argv[])
{

    if (argc != 1){
        auto nw = atoi(argv[1]);

        const int n = 1024;
        std::vector<int> v(n);
        auto actualSum = 0;
        for(int i=0; i<n; i++){
            v[i] = i+1;
            actualSum += v[i];
        }

        int sum = 0;
        #pragma omp parallel for num_threads(nw) reduction(+:sum)
            for (int i = 0; i < n; i++){
                sum += v[i];
            }
        
        
        std::cout << "Sum is " << sum << " should be " << actualSum << std::endl;
    }

    return 0;
}