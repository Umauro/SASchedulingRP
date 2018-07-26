#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>

#include "scheduler.h"


int main(int args, char **argv){
    if(args != 2){
        std::cout << "Cantidad de Parámetros Incorrectos";
        return 1;
    }
    Scheduler scheduler = Scheduler();
    if(scheduler.leerInstancia(argv[1])){
        return 1;
    };
    scheduler.constructorSolucion();
    scheduler.printSolucion();
    std::cout << "No Asignados: " << scheduler.noAsignados.size() << "\n";
    return 0;
}
