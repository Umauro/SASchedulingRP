#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <list>
#include <random>
#include <cstdlib>
#include <cmath>

#include "scheduler.h"
#include "paciente.h"

struct sortComparator;
struct sortNoAsignado;
struct sortAsignado;

class Scheduler{
public:
    int iter;
    int param1;
    float paramProb;
    float temp;
    float multi;
    int iterTemp;
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
    std::vector<int> capacidadMaquinas;
    float mejorSolucion;

    Scheduler(int iteraciones, int parametro1, float probabilidad, float temperatura, float multiplicador, int iterTemperatura);
    int leerInstancia(std::string instancia);
    bool compPrimeraCapacidad(int j, Paciente &paciente, std::vector<int> &capacidades);
    bool compCapacidad(int j, Paciente &paciente, std::vector<int> &capacidades);
    int diaAsigIncompleta(int release, Paciente &paciente, std::vector<int> &capacidades);
    int diaAsigCompleta(int release, Paciente &paciente, std::vector<int> &capacidades);
    void asignar(int diaAsig, Paciente &paciente, std::vector<int> &capacidades);
    void ASAP(Paciente &paciente, std::vector<int> &capacidades, std::vector<Paciente> &asig, std::vector<Paciente> &noAsig);
    void JIP(Paciente paciente);
    float funcionObjetivo();
    float funcionObjetivo(std::vector<Paciente> candidato);
    void constructorSolucion();
    void metricas();
    void recalculador(std::vector<int> &capacidades, Paciente &paciente);
    void localSearch();
    void printSolucion();
    void debugger();
};
#endif
