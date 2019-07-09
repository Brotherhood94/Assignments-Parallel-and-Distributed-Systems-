/* 
    Parallel Prefix parallelo, si divide in partizioni l'array
    e si calcola il parallel prefix di ogni singola partizione
    poi calcoliamo il prefissop del vettore completo aggiungendo
    l'ultimo prefisso di ogni partizione ai valori della partizione successiva.
*/

#include <iostream>
#include <vector>
#include "utimer.h"
#include <thread>
using namespace std;

/*
    Calcolo dei prefissi locali
*/
void phase1(vector<int> &x,                // the input/output vector
            vector<int> &p,                // the prefix vector
            int i,                         // the partition index
            function<int(int, int)> oplus, // the combiner function
            int nw                         // the parallelism degree
){
    auto m = x.size();
    auto delta = m / nw;
    // phase one, compute prefix of the assigned partition
    for (int j = i * delta + 1; j < (i + 1) * delta; j++)
        x[j] = oplus(x[j - 1], x[j]);
    // and assign final item to the prefix vector
    p[i] = x[(i + 1) * delta - 1];
    return;
}

// Funzione per il calcolo delle somme prefisse
void prefix(vector<int> &x, function<int(int, int)> oplus)
{
    for (int i = 1; i < x.size(); i++)
        x[i] = oplus(x[i], x[i - 1]);
    return;
}

// Stampa del vettore
void printv(vector<int> x)
{
    for (int i = 0; i < x.size(); i++)
        cout << x[i] << " ";
    cout << endl;
    return;
}

// Funzione che calcola la somma
int oplus(int x, int y)
{
    auto temp = x + y;
    // possibly we will fill here some delay/longer computation
    return temp;
}
/* 
    Update delle partizioni
*/
void phase2(vector<int> &x, vector<int> &p, int i, function<int(int, int)> oplus, int nw)
{
    auto m = x.size();
    auto delta = m / nw;
    for (int j = i * delta; j < (i + 1) * delta; j++)
        x[j] = oplus(x[j], p[i - 1]);
    return;
}

/*
    Funzione che crea i thread per gestire la prima 
    e la seconda fase.
*/
void compute_prefix(vector<int> &x, function<int(int, int)> oplus, int nw){

    vector<thread *> tid(nw);
    vector<int> p(nw);

    // first create threads for phase 1
    for (int i = 0; i < nw; i++)
        tid[i] = new thread(phase1, ref(x), ref(p), i, oplus, nw);
    for (int i = 0; i < nw; i++)
        tid[i]->join();

    // then compute prefix of prefixes
    // Calcoliamo le somme prefisse su P
    for (int i = 1; i < nw; i++)
        p[i] = oplus(p[i - 1], p[i]);

    // Nella fase due sommiamo i valori di P ai vari blocchi
    for (int i = 1; i < nw; i++)
        tid[i] = new thread(phase2, ref(x), ref(p), i, oplus, nw);
    for (int i = 1; i < nw; i++)
        tid[i]->join();
        
    return;
}

int main(){
    vector<int> p(2);
    vector<int> q = {1, 2, 3, 4, 5, 6, 7, 8};
    // 0 è l'indice della partizione
    phase1(q, p, 0, oplus, 2);

    // Stampiamo prima il vettore con i risultati intermedi
    printv(q);
    // Poi il vettore p in cui segno solamente l'ultimo valore di ogni partizione
    printv(p);
    
    // 1 è l'indice della partizione
    phase1(q, p, 1, oplus, 2);
    printv(p);
    printv(q);

    // Qua calcoliamo la somma prefissa sul vettore
    // p ovvero sul vettore in cui abbiamo l'elemento finale
    // di ogni blocco
    prefix(p, oplus);
    printv(p);
    printv(q);


    // In parallelo
    std::cout << "Parallelo" << std::endl;
    vector<int> b(8, 1);
    compute_prefix(b, oplus, 2);
    printv(b);

    vector<int> q2 = {1, 2, 3, 4, 5, 6, 7, 8};
    compute_prefix(q2, oplus, 2);
    printv(q2);
}