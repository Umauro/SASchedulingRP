#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>

#include "paciente.h"

Paciente::Paciente(int ide, int categor, int rel,
     int du, int ses, int interrup, int tipoM,
     int tiempoS, int dias){
    this -> id = ide;
    this -> categoria = categor;
    this -> release = rel;
    this -> due = du;
    this -> sesiones = ses;
    this -> interrupciones = interrup;
    this -> tipoMaquina = tipoM;
    this -> tiempoPrimeraSesion = 5;
    this -> tiempoSesion = tiempoS;
    //int disponibilidad[5] = {a1, a2, a3, a4, a5};
    this ->  sesionesRestantes = ses;
    this -> schedulePaciente = std::vector<int>(dias,0);
    this -> tiempoEspera = -1;
    this -> inicio = -1; //Van a guardar índices
    this -> fin = -1; //Van a guardar índices
}
