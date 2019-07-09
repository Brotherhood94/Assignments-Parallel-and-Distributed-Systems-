#include <iostream>
#include <thread>


void body1(int n){

    for(int i=0;i<n;i++){
        std::cout << "This is thread n. ";
        std::cout << n << std::endl;
    }

    return; 
}


int main(int argc, char * argv[]){
    if (argc == 1)
    {
        std::cout << "Serve un parametro" << std::endl;
        return 0;
    }

    int nThread = atoi(argv[1]);
    
    

    // Creo il vettore con i thread ID
    std::thread * t[nThread];

    std::cout << "Running " << nThread << " Threads" << std::endl;

    // Assegno ad ogni posizione del vettore un nuovo thread 
    // ogni thread ha come parametro la funzione body che voglio 
    // che venga eseguita e poi viene passato come parametro i 
    for(int i=0;i<nThread;i++){
        std::cout << "Starting Thread " << i << std::endl;
        t[i] = new std::thread(body1,i);
    }

    // Serve la join per fare in modo che si aspetti 
    // la fine di ogni thread.

    for (int i = 0; i < nThread; i++)
    {
        //Serve la -> perchè è un puntatore
        t[i]->join(); 
       
    }

    return 0;
}