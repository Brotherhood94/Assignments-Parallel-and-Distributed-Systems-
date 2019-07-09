#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <omp.h>
#include <atomic>

using namespace std::literals::chrono_literals;

std::atomic<int> presenceBit;

int buffer;

int main(int argc, char * argv[]){
    presenceBit = 0;
    buffer = 0;
    auto delay = 1500ms;
    auto nw = atoi(argv[1]);
    const int n = 10;
    #pragma omp parallel num_threads(nw)
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                std::cout << "This is the producer section, it is thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << std::endl;
                for(int i=0;i<n;i++){
                    std:: cout << "Waiting buffer free to send ..." << std::endl;
                    while(presenceBit == 1){
                        std::this_thread::sleep_for(1ms);
                    } 
                    buffer = i;
                    presenceBit = 1;
                    std::cout << "Sent!" << std::endl;
                    std::this_thread::sleep_for(delay);
                }
            }

            #pragma omp section
            {
                std::cout << "This is the second section, it is thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << std::endl;
                for (int i = 0; i < n; i++)
                {
                    
                    while (presenceBit == 0)
                    {
                        std::this_thread::sleep_for(1ms);
                    }

                    std::cout << "Received " << buffer << std::endl;

                    presenceBit = 0;
                    std::this_thread::sleep_for(delay);
                }
            }
        }
        
    }

    return 0;
}
