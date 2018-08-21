#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>

#include "scheduler.h"


int main(int args, char **argv){

    float theVerybest = 10000000;
    std::vector<Paciente> asignadosBest;
    std::vector<Paciente> noAsignadosBest;
    std::vector<int> capacidadBest;
    unsigned int capacidadMinimaBest = 0;
    int graspIter = 0;
    if(args != 9){
        std::cout << "Cantidad de Parámetros Incorrectos";
        return 1;
    }
    Scheduler scheduler = Scheduler(atoi(argv[2]),atoi(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atoi(argv[7]));
    if(scheduler.leerInstancia(argv[1])){
        return 1;
    };

    scheduler.constructorSolucion();
    graspIter = atoi(argv[8]);
    //std::cout << "\nAsignados: " << scheduler.asignados.size() << "\n";
    //std::cout << "No Asignados: " << scheduler.noAsignados.size() << "\n";
    //std::cout << "Avg waiting time: " << scheduler.funcionObjetivo() <<"\n";
    //scheduler.metricas();
    //scheduler.printSolucion();

    theVerybest = scheduler.mejorSolucion;
    asignadosBest = scheduler.asignados;
    noAsignadosBest = scheduler.noAsignados;
    capacidadBest = scheduler.capacidadMaquinas;
    capacidadMinimaBest = asignadosBest.size();

    //std::cout << "iteracion AvgTime noAsignado\n";
    for(int i = 0; i < graspIter; i ++){
        scheduler.constructorSolucion();
        scheduler.localSearch();

        if(scheduler.mejorSolucion < theVerybest && scheduler.asignados.size()>= capacidadMinimaBest){
            theVerybest = scheduler.mejorSolucion;
            asignadosBest = scheduler.asignados;
            noAsignadosBest = scheduler.noAsignados;
            capacidadBest = scheduler.capacidadMaquinas;
            capacidadMinimaBest = scheduler.asignados.size();
        }
        //std::cout << i << " "<< theVerybest << " " << noAsignadosBest.size() << "\n";
    }

    scheduler.asignados = asignadosBest;
    scheduler.noAsignados = noAsignadosBest;


    //scheduler.metricas();
    std::cout << "\n";
    //scheduler.localSearch();
    //scheduler.printSolucion();

    std::cout << "Asignados: " << asignadosBest.size() << "\n";
    std::cout << "No Asignados: " << noAsignadosBest.size() << "\n";
    std::cout << "Avg waiting time: " << scheduler.funcionObjetivo(asignadosBest) << "\n";
    scheduler.metricas();
    //std::cout << scheduler.funcionObjetivo() << " " << scheduler.asignados.size() << " ";
    //scheduler.debugger();
    //std::cout << theVerybest << " " << noAsignadosBest.size() << " ";
    //scheduler.metricas();
    return 0;
}
