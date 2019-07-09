/* 
    Parallel Prefix nel caso sequenziale, in questo caso 
    semplicemente si somma la posizione i-1 del vettore con 
    la posizione i del vettore scorrendo tutto il vettore e poi
    in questo modo abbiamo il vettore finale con le somme prefisse.
*/

#include <iostream>
#include <vector>
#include "utimer.h"

using namespace std;

// Vettore da calcolare
vector<int> y = {1, 2, 3, 4, 5, 6, 7, 8};

// Funzione per attesa attiva
void active_udelay(int usecs){
    // read current time
    auto start = std::chrono::high_resolution_clock::now();
    auto end = false;
    while (!end)
    {
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        if (usec > usecs)
            end = true;
    }
    return;
}

// Funzione che calcola la somma
int oplus(int x, int y){
    auto temp = x + y;
    // possibly we will fill here some delay/longer computation
    return temp;
}

// Calcolo della somma con attesa attiva
int oplus2(int x, int y){
    auto temp = x + y;
    active_udelay(1000);
    return temp;
}

// Funzione per il calcolo delle somme prefisse
void prefix(vector<int> &x, function<int(int, int)> oplus){
    for (int i = 1; i < x.size(); i++)
        x[i] = oplus(x[i], x[i - 1]);
    return;
}

// Stampa del vettore
void printv(vector<int> x){
    for (int i = 0; i < x.size(); i++)
        cout << x[i] << " ";
    cout << endl;
    return;
}

int main(){
    // Esecuzione sequenziale sul vettore y
    printv(y);
    prefix(y, oplus);
    printv(y);

    /*  Calcolo sequenziale su x con conteggio del tempo
        utimer t viene creato tra le parentesi perchè
        prima viene creato il timer e poi quando arrivo
        alla parentesi che viene chiusa l'oggetto viene 
        distrutto usando il metodo distruttore della classe utimer.
    */
    vector<int> x = {1, 2, 3, 4, 5, 6, 7, 8};
    
    {
    utimer t("prefix8");
    prefix(x, oplus);
    }

    // Calcolo sequenziale con attesa attiva
    vector<int> w = {1, 1, 1, 1, 1, 1, 1, 1};
    {
        utimer t3("now");
        prefix(w, oplus2);
    }

    // Calcolo sequenziale su un vettore di 1024 elementi con calcolo del tempo
    vector<int> longx(1024);
    for (auto &i : longx)
        i = 1;

    {
        utimer t2("longx");
        prefix(longx, oplus);
    }

    // Calcolo sul vettore più grande con attesa attiva
    const int init = 1;
    vector<int> d(1024);
    for (auto &i : d)
        i = 2;
    {
        utimer t("longy");
        prefix(d, oplus2);
    }
}