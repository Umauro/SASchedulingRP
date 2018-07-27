#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <list>

#include "scheduler.h"
#include "paciente.h"

struct sortComparator;
struct sortNoAsignado;
struct sortAsignado;

class Scheduler{
public:
    int dias;
    int diasTrabajo;
    int turnos;
    int tiempo;
    int cantidadMaquina1;
    int cantidadMaquina2;
    int cantidadUrgent;
    int cantidadPalliative;
    int cantidadRadical;
    std::vector<Paciente> pacientes;
    std::vector<Paciente> noAsignados;
    std::vector<Paciente> asignados;
    std::vector<int> schedule;
    std::vector<int> capacidadMaquinas;

    Scheduler();
    int leerInstancia(std::string instancia);
    bool compPrimeraCapacidad(int j, Paciente &paciente);
    bool compCapacidad(int j, Paciente &paciente);
    int diaAsigIncompleta(int release, Paciente &paciente);
    int diaAsigCompleta(int release, Paciente &paciente);
    void asignar(int diaAsig, Paciente &paciente);
    void ASAP(Paciente &paciente);
    void JIP(Paciente paciente);
    void constructorSolucion();
    float funcionObjetivo();
    void metricas();
    void recalculador(std::vector<int> &capacidades, Paciente &paciente);
    void printSolucion();
};
#endif
