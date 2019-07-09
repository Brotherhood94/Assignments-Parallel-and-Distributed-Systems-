#include <iostream>
#include <vector>
#include <functional>
#include <experimental/optional>
#include <grppi/common/patterns.h>
#include <grppi.h>
#include <dyn/dynamic_execution.h>
#include "utimer.hpp"

using namespace std;
using namespace std::literals::chrono_literals;


/*
    Drain in versione funzione.
*/
void drain(int x) {
    this_thread::sleep_for(10ms);
    cout << x << endl;
}

int
main(int argc, char *argv[])
{

    auto m = atoi(argv[1]);
    auto nw = atoi(argv[2]);
    /*
        Queste variabili mi servono per dire nella pipeline se l'esecuzione
        deve essere sequenziale o parallela e, nel caso in cui dovesse
        essere parallela indichiamo il backend da utilizzare.
    */
    grppi::dynamic_execution seq = grppi::sequential_execution{};
    grppi::dynamic_execution thr = grppi::parallel_execution_native{};
    grppi::dynamic_execution omp = grppi::parallel_execution_omp{};

    /*
        Closure che prende dallo stream i dati.
        Nella pipeline metto semplicemente il nome.
        Questo drain posso anche dichiararlo fuori come una normale funzione
    */
    auto drain = [](int x) {
        this_thread::sleep_for(10ms);
        cout << x << endl;
    };

    /*
        Creiamo due nuove funzioni per due nuovi stage della pipeline.
        Ora facciamo in modo che uno dei due ci mette più tempo dell'altro.
        Compilando con queste funzioni nella pipeline ci mette un po' di tempo in più.
        Per aumentare la velocità di questo s2 possiamo introdurre una farm.
    */
    auto s1 = [](int x) {
        this_thread::sleep_for(10ms);
        return ++x;
    };

    auto s2 = [](int x) {
        this_thread::sleep_for(50ms);
        return x*x;
    };

    {
        utimer t("Pipeline execution");

        /*
            Creiamo una pipeline in cui c'è uno stage che produce uno stream e 
            uno stage in cui invece consumiamo lo stream.
            Grppi è il namespace per Grppi, viene creata una pipeline con i seguenti parametri:
            - Seq: indica che l'esecuzione sarà sequenziale
            - La prima funzione è una lambda, non ha argomenti, [m] ha il valore della variabile
            m nell'ambiente. Definisco il tipo del risultato come optional<int>.
            - Il second stage: abbiamo il concetto della singola funzione che modifica il singolo element
            dello stream. Le funzioni ricevono un intero quando devono leggere dallo stream, option
            non deve essere gestito, viene gestito dall'implementazione. Il secondo stage manda
            in output i numeri da 0 a m.
        */
        grppi::pipeline( //execution engine
            seq,
            [m]() -> experimental::optional<int> {
                static int x = 0;
                this_thread::sleep_for(10ms);
                if (x < m)
                    return x++;
                else
                    return {};
            },
            s1,
            // Introduciamo una farm in cui abbiamo nw worker e ognuno svolge la funzione s2
            grppi::farm(nw,s2),
            // stream collapser stage
            drain
            // end of the pipeline parameters
        );
    }
    return 0;
}