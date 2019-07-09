#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <vector>
#include <thread>

#include "Translator.hpp"

using namespace std;

typedef struct{
    int start;
    int end;
} RANGE;

int main(int argc, char * argv[]){

    if (argc == 1){
        std::cout << "Serve un parametro" << std::endl;
        return 0;
    }

    string filename = argv[1];

    // Per prima cosa viene aperto il file, poi creiamo text in cui 
    // salviamo il testo e poi abbiamo line che verrà usata per
    // accumulare il testo che leggo prima di appenderlo a text.
    ifstream fd(filename);
    string text = "";
    string line;

    while(getline(fd,line)){
        text.append(line);
        text.append("\n");
    }
    fd.close();

    // primo caso, vogliamo eseguire la traduzione sequenziale
    if(argc == 2){
        auto start = std::chrono::high_resolution_clock::now();

        // la funzione transform prende come parametro un range, nel nostro caso begin e end
        // poi un secondo punto in cui verrà salvato l'output, nel nostro caso questo è il 
        // secondo begin. Poi prende una funzione che prende in input i vari elementi del range
        transform(text.begin(), text.end(), text.begin(), translate_char);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout << "Spent " << usec << " usec to translate " << filename << " Sequentially" << endl;
    }
    // In questo caso vogliamo considerare una esecuzione in parallelo della traduzione
    else{
        // Quando avviamo dobbiamo indicare quanto vogliamo andare in parallelo
        int nw = atoi(argv[2]);
        
        /*
            Definizione di una lambda in C++.
            Non deve essere indicato il tipo della funzione quindi mettiamo auto
            poi mettiamo il nome della funzione, dopo l'uguale abbiamo tra le parentesi
            tonde il parametro che deve essere indicato quando chiamo la funzione
            invece tra le parentesi quadrate c'è un parametro che viene preso dall'ambiente
            in questo caso si tratta della variabile text che viene passata come riferimento.
            Nel body della lambda abbiamo il for che lavora su text ed effettua la modifica al testo
            chiamando la funzione definita in Translator.hpp.

            Nel nostro caso la funzione compute_chunk riceve un range di testo su cui lavorare e 
            modifica quello specifico testo. L'idea è di chiamarla in parallelo da vari thread in modo
            da effettuare le modifiche in parallelo.
        */
        auto compute_chunk = [&text](RANGE range) {
            for(int i = range.start; i < range.end; i++)
            {
                text[i] = translate_char(text[i]);
            }
            return;
        };

        auto start = std::chrono::high_resolution_clock::now();
        vector<RANGE> ranges(nw);
        int m = text.size();
        int delta {m/nw};
        vector<thread> tids;

        // L'array dei range è fatto in modo di dividere in blocchi l'array
        for(int i = 0; i<nw; i++){
            ranges[i].start = i*delta;
            // Questo qua sotto è un if, prima del ? abbiamo la condizione 
            // dopo le due possibili soluzioni
            ranges[i].end = (i != (nw-1) ? (i+1)*delta : m);
        }

        // Creiamo i vari thread inserendo all'interno del vettore i thread ID dei 
        // vari thread che creiamo. Quando creiamo il thread gli indichiamo la funzione
        // che deve essere eseguita e poi passiamo il parametro che in questo caso è
        // il range i.
        for(int i=0; i<nw; i++){
            tids.push_back(thread(compute_chunk,ranges[i]));
        }

        for (thread & t : tids){
            t.join();
        }

        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout << "Spent " << usec << " usec to translate " << filename << " In parallel with " << nw << " Threads" << endl;
    }

    return 0;
}