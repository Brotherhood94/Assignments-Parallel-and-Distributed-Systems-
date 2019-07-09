#include <iostream>
#include <omp.h>

int main(int argc, char *argv[])
{

    if (argc != 1)
    {
        auto nw = atoi(argv[1]);
        #pragma omp parallel num_threads(nw)
        {
            #pragma omp sections
            {
                #pragma omp section
                {
                    std::cout << "This is first section, thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << std::endl;
                }

                #pragma omp section
                {
                    std::cout << "This is second section, thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << std::endl;
                }
            }
        }
    }
    
    return 0;
}