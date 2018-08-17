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

// Override () operator for sorting //
struct sortComparator{
    bool operator ()(Paciente const a, Paciente const b) const{
        return (a.release < b.release) ||
                ((a.release == b.release)  &&
                    ((a.categoria > b.categoria) ||
                        ((a.categoria == b.categoria) &&
                            (a.sesiones > b.sesiones))));
    }
};

struct sortNoAsignado{
    bool operator ()(Paciente const a, Paciente const b) const{
        return (a.sesiones > b.sesiones) ||
                ((a.sesiones == b.sesiones) &&
                    ((a.release > b.release) ||
                        ((a.release == b.release) &&
                            (a.categoria < b.categoria))));
    }
};

struct sortAsignado{
    bool operator ()(Paciente const a, Paciente const b) const{
        return (a.sesiones < b.sesiones) ||
                ((a.sesiones == b.sesiones) &&
                    ((a.tiempoEspera < b.tiempoEspera) &&
                        ((a.tiempoEspera == b.tiempoEspera) &&
                            (a.release > b.release))));
    }
};

Scheduler::Scheduler(int iteraciones, int parametro1, float probabilidad, float temperatura, float multiplicador, int iterTemperatura){
    iter = iteraciones;
    param1 = parametro1;
    paramProb = probabilidad;
    temp = temperatura;
    multi = multiplicador;
    iterTemp = iterTemperatura;
    dias = 0;
    diasTrabajo = 0;
    turnos = 0;
    tiempo = 0;
    cantidadMaquina1 = 0;
    cantidadMaquina2 = 0;
    cantidadUrgent = 0;
    cantidadPalliative = 0;
    cantidadRadical = 0;
    pacientes = std::vector<Paciente>();
    noAsignados = std::vector<Paciente>();
    asignados = std::vector<Paciente>();
}

int Scheduler::leerInstancia(std::string instancia){
    int e1, e2, e3, e4, e5, e6, e7, test;
    int idCounter = 1;
    int urgentCounter = 0;
    int palliativeCounter = 0;
    int radicalCounter = 0;
    std::ifstream archivo(instancia);
    if(!archivo.good()){
        std::cout << "Error al leer la instancia \n";
        return 1;
    }
    archivo >> e1 >> e2 >> e6 >> e7;
    this -> dias = e1;
    //this -> diasTrabajo = e2;
    //this -> turnos = e3;
    this -> tiempo = e2;
    this -> cantidadMaquina1 = e6;
    this -> cantidadMaquina2 = e7;

    while(archivo >> test){
        for(int i = 0; i < test; i++){
            archivo >> e1 >> e2 >> e3 >> e4 >> e5 >> e6 >> e7;

            Paciente paciente(idCounter, e1, e2, e3, e4, e5, e6, e7, dias);
            pacientes.push_back(paciente);
            if(e1 == 1){
                urgentCounter++;
            }
            else if(e1 == 2){
                palliativeCounter++;
            }
            else{
                radicalCounter++;
            }
            idCounter++;
        }
    }
    archivo.close();
    cantidadUrgent = urgentCounter;
    cantidadPalliative = palliativeCounter;
    cantidadRadical = radicalCounter;
    return 0;
}

bool Scheduler::compPrimeraCapacidad(int j, Paciente &paciente, std::vector<int> &capacidades){
    if(paciente.tipoMaquina == 2){
        if((capacidades[j + dias] - (paciente.tiempoSesion+paciente.tiempoPrimeraSesion)) < 0){
            return false;
        }
        else{
            return true;
        }
    }
    else{
        if((capacidades[j] - (paciente.tiempoPrimeraSesion +paciente.tiempoSesion)) < 0){
            return false;
        }
        else{
            return true;
        }
    }
}

bool Scheduler::compCapacidad(int j, Paciente &paciente, std::vector<int> &capacidades){
    if(paciente.tipoMaquina == 2){
        if((capacidades[j + dias] - paciente.tiempoSesion) < 0){
            return false;
        }
        else{
            return true;
        }
    }
    else{
        if((capacidades[j] - paciente.tiempoSesion) < 0){
            return false;
        }
        else{
            return true;
        }
    }
}

int Scheduler::diaAsigIncompleta(int release, Paciente &paciente, std::vector<int> &capacidades){
    bool flag;
    bool primera;
    bool asignable = false;
    // Contador para ver cuantas sesiones han sido asignadas //
    int counter;
    for(int i = (release - 1) ; i < dias; i++){
        flag = true;
        primera = true;
        counter = 0;
        for(int j = i; j < dias; j ++){
            if(primera){
                /* Si el paciente tiene 5 sesiones o menos, su primer día de tratamiento
                 debe ser tal que alcance a tener todas sus sesiones el mismo día*/
                if(paciente.sesiones < 6){
                    if(5 - paciente.sesiones > (j % 7)){
                        flag = false;
                        break;
                    }
                }
                /*Si el paciente no es de categoría Urgente, no puede partir un viernes*/
                if(paciente.categoria != 1){
                    if(j % 7 == 4){
                        flag = false;
                        break;
                    }
                }
                /* No se puede partir el tratamiento los días viernes, sábados, o domingos */
                if(j % 7 == 5 || j % 7 == 6){
                    flag = false;
                    break;
                }
                /* Si es 1, es posible asignar la primera sesión en el día j*/
                if(compPrimeraCapacidad(j, paciente, capacidades)){
                    primera = false;
                    counter++;
                }
                /* Si no, se debe empezar con otro día*/
                else{
                    flag = false;
                    break;
                }
            }
            else{
                /*debo implementar un contador de sesiones*/
                if(j % 7 == 5 || j % 7 == 6){
                    continue;
                }
                if(compCapacidad(j, paciente, capacidades)){
                    primera = false;
                    counter++;
                }
                else{
                    flag = false;
                    break;
                }
            }
            /* Si se pueden asignar todas las sesiones del paciente se procede a
                realizar la asignación */
            if(counter == paciente.sesiones){
                asignable = true;
                break;
            }
        }
        if(flag && asignable){
            return i;
        }
    }
    //std::cout << "id no asignado: " << paciente.id << "\n";
    return 0;
}
int Scheduler::diaAsigIncompleta(int release, Paciente &paciente, std::vector<int> &capacidades, int due){
    bool flag;
    bool primera;
    bool asignable = false;
    int counter;
    if(due > dias){
        for(int i = (dias - 1) ; i > release; i--){
            flag = true;
            primera = true;
            counter = 0;
            for(int j = i; j < dias; j ++){
                if(primera){
                    /* Si el paciente tiene 5 sesiones o menos, su primer día de tratamiento
                     debe ser tal que alcance a tener todas sus sesiones el mismo día*/
                    if(paciente.sesiones < 6){
                        if(5 - paciente.sesiones > (j % 7)){
                            flag = false;
                            break;
                        }
                    }
                    /*Si el paciente no es de categoría Urgente, no puede partir un viernes*/
                    if(paciente.categoria != 1){
                        if(j % 7 == 4){
                            flag = false;
                            break;
                        }
                    }
                    /* No se puede partir el tratamiento los días viernes, sábados, o domingos */
                    if(j % 7 == 5 || j % 7 == 6){
                        flag = false;
                        break;
                    }
                    /* Si es 1, es posible asignar la primera sesión en el día j*/
                    if(compPrimeraCapacidad(j, paciente, capacidades)){
                        primera = false;
                        counter++;
                    }
                    /* Si no, se debe empezar con otro día*/
                    else{
                        flag = false;
                        break;
                    }
                }
                else{
                    if(j % 7 == 5 || j % 7 == 6){
                        continue;
                    }
                    if(compCapacidad(j, paciente, capacidades)){
                        primera = false;
                        counter++;
                    }
                    else{
                        flag = false;
                        break;
                    }
                }
                if(counter == paciente.sesiones){
                    asignable = true;
                    break;
                }
            }

            if(flag && asignable){
                return i;
            }
        }
    }
    else{
        for(int i = (due - 1) ; i > release; i--){
            flag = true;
            primera = true;
            counter = 0;
            for(int j = i; j < dias; j ++){
                if(primera){
                    /* Si el paciente tiene 5 sesiones o menos, su primer día de tratamiento
                     debe ser tal que alcance a tener todas sus sesiones el mismo día*/
                    if(paciente.sesiones < 6){
                        if(5 - paciente.sesiones > (j % 7)){
                            flag = false;
                            break;
                        }
                    }
                    /*Si el paciente no es de categoría Urgente, no puede partir un viernes*/
                    if(paciente.categoria != 1){
                        if(j % 7 == 4){
                            flag = false;
                            break;
                        }
                    }
                    /* No se puede partir el tratamiento los días viernes, sábados, o domingos */
                    if(j % 7 == 5 || j % 7 == 6){
                        flag = false;
                        break;
                    }
                    /* Si es 1, es posible asignar la primera sesión en el día j*/
                    if(compPrimeraCapacidad(j, paciente, capacidades)){
                        counter++;
                        primera = false;
                    }
                    /* Si no, se debe empezar con otro día*/
                    else{
                        flag = false;
                        break;
                    }
                }
                else{
                    if(j % 7 == 5 || j % 7 == 6){
                        continue;
                    }
                    if(compCapacidad(j, paciente, capacidades)){
                        counter++;
                        primera = false;
                    }
                    else{
                        flag = false;
                        break;
                    }
                }
                if(counter == paciente.sesiones){
                    asignable = true;
                    break;
                }
            }
            if(flag && asignable){
                return i;
            }
        }
    }
    //std::cout << "id no asignado: " << paciente.id << "\n";
    return 0;
}
int Scheduler::diaAsigCompleta(int release, Paciente &paciente, std::vector<int> &capacidades){
    bool flag;
    bool primera;
    bool asignable = false;
    int counter;
    for(int i = (release - 1) ; i < paciente.due; i++){
        flag = true;
        primera = true;
        counter = 0;
        for(int j = i; j < dias; j ++){
            if(primera){
                /* Si el paciente tiene 5 sesiones o menos, su primer día de tratamiento
                 debe ser tal que alcance a tener todas sus sesiones el mismo día*/
                if(paciente.sesiones < 6){
                    if(5 - paciente.sesiones > (j % 7)){
                        flag = false;
                        break;
                    }
                }
                /*Si el paciente no es de categoría Urgente, no puede partir un viernes*/
                if(paciente.categoria != 1){
                    if(j % 7 == 4){
                        flag = false;
                        break;
                    }
                }
                /* No se puede partir el tratamiento los días viernes, sábados, o domingos */
                if(j % 7 == 5 || j % 7 == 6){
                    flag = false;
                    break;
                }
                /* Si es 1, es posible asignar la primera sesión en el día j*/
                if(compPrimeraCapacidad(j, paciente, capacidades)){
                    counter++;
                    primera = false;
                }
                /* Si no, se debe empezar con otro día*/
                else{
                    flag = false;
                    break;
                }
            }
            else{
                if(j % 7 == 6 || j % 7 == 0){
                    continue;
                }
                if(compCapacidad(j, paciente, capacidades)){
                    counter++;
                    primera = false;
                }
                else{
                    flag = false;
                    break;
                }
            }
            if(counter == paciente.sesiones){
                asignable = true;
                break;
            }
        }
        if(flag && asignable){
            return i;
        }
    }
    //std::cout << "id no asignado: " << paciente.id << "\n";
    return 0;
}

int Scheduler::diaAsigCompleta(int release, Paciente &paciente, std::vector<int> &capacidades, int due){
    bool flag;
    bool primera;
    bool asignable = false;
    int counter;
    for(int i = (due - 1) ; i > release; i--){
        flag = true;
        primera = true;
        counter = 0;
        for(int j = i; j < dias; j ++){
            //std::cout << "j: "<<j <<" i:"<< i<<  "\n";
            if(primera){
                /* Si el paciente tiene 5 sesiones o menos, su primer día de tratamiento
                 debe ser tal que alcance a tener todas sus sesiones el mismo día*/
                if(paciente.sesiones < 6){
                    if(5 - paciente.sesiones > (j % 7)){
                        flag = false;
                        break;
                    }
                }
                /*Si el paciente no es de categoría Urgente, no puede partir un viernes*/
                if(paciente.categoria != 1){
                    if(j % 7 == 4){
                        flag = false;
                        break;
                    }
                }
                /* No se puede partir el tratamiento los días viernes, sábados, o domingos */
                if(j % 7 == 5 || j % 7 == 6){
                    flag = false;
                    break;
                }
                /* Si es 1, es posible asignar la primera sesión en el día j*/
                if(compPrimeraCapacidad(j, paciente, capacidades)){
                    primera = false;
                    counter++;
                }
                /* Si no, se debe empezar con otro día*/
                else{
                    flag = false;
                    break;
                }
            }
            else{
                if(j % 7 == 6 || j % 7 == 0){
                    continue;
                }
                if(compCapacidad(j, paciente, capacidades)){
                    primera = false;
                    counter++;
                }
                else{
                    flag = false;
                    break;
                }
            }
            if(counter == paciente.sesiones){
                asignable = true;
                break;
            }
        }
        if(flag && asignable){
            return i;
        }
    }
    //std::cout << "id no asignado: " << paciente.id << "\n";
    return 0;
}

void Scheduler::asignar(int diaAsig, Paciente &paciente, std::vector<int> &capacidades){
    int contador = 0;
    bool primera = true;
    for(int j = diaAsig; j < dias; j ++){
        if(contador == 0) paciente.inicio = j; //Se guarda el indice del inicio
        if(contador == paciente.sesiones){
            paciente.fin = j; //Se guarda el índice del fin
            break;
        }
        if(j % 7 == 5 || j % 7 == 6){
            continue;
        }
        paciente.schedulePaciente[j] = 1;
        paciente.tiempoEspera = diaAsig - (paciente.release -1);
        contador++;
        if(paciente.tipoMaquina == 2){
            if(primera){
                capacidades[j + dias] -= (paciente.tiempoSesion+paciente.tiempoPrimeraSesion);
                primera = false;
            }
            else{
                capacidades[j + dias] -= paciente.tiempoSesion;
            }
        }
        else{
            if(primera){
                capacidades[j] -= (paciente.tiempoSesion+paciente.tiempoPrimeraSesion);
                primera = false;
            }
            else{
                capacidades[j] -= paciente.tiempoSesion;
            }
        }
    }
    if(paciente.fin == -1){
        paciente.fin = dias - 1;
    }
}


void Scheduler::ASAP(Paciente &paciente, std::vector<int> &capacidades, std::vector<Paciente> &asig, std::vector<Paciente> &noAsig){
    int release = paciente.release;
    int due = paciente.due;
    int diaAsig = 0;
    if(paciente.sesiones > (dias - due)){
        diaAsig = diaAsigIncompleta(release, paciente, capacidades);
        if(diaAsig){
            asignar(diaAsig, paciente, capacidades);
            asig.push_back(paciente);
        }
        else{
            noAsig.push_back(paciente);
        }
    }
    else{
        diaAsig = diaAsigCompleta(release, paciente, capacidades);
        if(diaAsig){
            asignar(diaAsig, paciente, capacidades);
            asig.push_back(paciente);
        }
        else{
            noAsig.push_back(paciente);
        }
    }
}

void Scheduler::JIP(Paciente &paciente, std::vector<int> &capacidades, std::vector<Paciente> &asig, std::vector<Paciente> &noAsig){
    int release = paciente.release;
    int due = paciente.due;
    int diaAsig = 0;
    if(paciente.sesiones > (dias - due)){
        diaAsig = diaAsigIncompleta(release, paciente, capacidades, due);
        //std::cout << diaAsig << "\n";
        if(diaAsig){
            asignar(diaAsig, paciente, capacidades);
            asig.push_back(paciente);
        }
        else{
            noAsig.push_back(paciente);
        }
    }
    else{
        diaAsig = diaAsigCompleta(release, paciente, capacidades, due);
        if(diaAsig){
            asignar(diaAsig, paciente, capacidades);
            asig.push_back(paciente);
        }
        else{
            noAsig.push_back(paciente);
        }
    }
}

float Scheduler::funcionObjetivo(){
    float suma = 0;
    for(auto &i:asignados){
        suma += i.tiempoEspera;
    }
    return (suma/asignados.size());
}

float Scheduler::funcionObjetivo(std::vector<Paciente> candidato){
    float suma = 0;
    for(auto &i:candidato){
        suma += i.tiempoEspera;
    }
    return (suma/candidato.size());
}

void Scheduler::constructorSolucion(){
    std::sort(pacientes.begin(), pacientes.end(), sortComparator());
    int largoLista = (cantidadMaquina1+cantidadMaquina2)*dias;
    capacidadMaquinas = std::vector<int>(largoLista, tiempo);
    std::random_device rd;
    std::mt19937 generadora(rd());
    std::uniform_real_distribution<> proba(0,1.0);
    float probabilidad;
    for(auto &i:pacientes){
        probabilidad = proba(generadora);
        //std::cout << probabilidad << "\n";
        if(probabilidad< 0.8){
            ASAP(i, capacidadMaquinas, asignados, noAsignados);
        }
        else{
            JIP(i, capacidadMaquinas, asignados, noAsignados);
        }
    }
    mejorSolucion = funcionObjetivo();
}

void Scheduler::metricas(){
    float urgent = 0;
    float palliative = 0;
    float radical = 0;
    for(auto &i: noAsignados){
        if(i.categoria == 1){
            urgent += 1;
        }
        else if(i.categoria == 2){
            palliative += 1;
        }
        else{
            radical += 1;
        }
    }
    //std::cout << (urgent/cantidadUrgent) << " " << (palliative/cantidadPalliative) << " " << (radical/cantidadRadical) << " " << ((urgent+palliative+radical)/pacientes.size()) <<"\n";
    std::cout << "Waiting urgent: " << (urgent/cantidadUrgent) << "% \n";
    std::cout << "Waiting palliative: " << (palliative/cantidadPalliative) << "% \n";
    std::cout << "Waiting radical: " << (radical/cantidadRadical) << "% \n";
    std::cout << "Total: " << ((urgent+palliative+radical)/pacientes.size()) << "% \n";
}

void Scheduler::recalculador(std::vector<int> &capacidades, Paciente &paciente){
    //Se actualizará el contador de capacidad de las máquinas desde el inicio al
    //fin del tratamiento de un paciente.
    //Se utiliza cuando se saca un paciente de la lista de asignados
    int counter = 0;
    if(paciente.tipoMaquina == 2){
        capacidades[paciente.inicio + dias] += (paciente.tiempoSesion + paciente.tiempoPrimeraSesion);
        counter++;
        for(int i = paciente.inicio + 1; i < paciente.fin; i++){
            if(i % 7 == 5 || i % 7 == 6){
                continue;
            }
            capacidades[i + dias] += paciente.tiempoSesion;
            counter++;
        }
        //std::cout << "Counter: " << counter << " Sesiones: " << paciente.sesiones << " Release: "<< paciente.release <<"\n";
    }
    else{
        capacidades[paciente.inicio] += (paciente.tiempoSesion + paciente.tiempoPrimeraSesion);
        for(int i = paciente.inicio + 1; i < paciente.fin; i++){
            if(i % 7 == 5 || i % 7 == 6){
                continue;
            }
            capacidades[i] += paciente.tiempoSesion;
            //std::cout << i << " " << paciente.fin << "\n";
        }

    }
    //Reseteo del paciente que fue sacado
    paciente.schedulePaciente = std::vector<int>(dias, 0);
    paciente.inicio = -1;
    paciente.fin = -1;
}

void Scheduler::localSearch(){
    unsigned int cantidadMinima = asignados.size();
    float delta;
    int randomNumber;
    float randomProb;
    float fitNuevo;
    std::random_device rd;
    std::mt19937 generador(rd());
    //std::default_random_engine generador;
    std::uniform_int_distribution<int> distribucion(1, param1);
    std::uniform_real_distribution<> prob(0,1.0);

    // Para guardar la solución actual
    float solucionActual = mejorSolucion;
    std::vector<Paciente> asignadosActual = asignados;
    std::vector<Paciente> noAsignadosActual = noAsignados;
    std::vector<int> capacidadActual = capacidadMaquinas;

    for(int iteracion = 0; iteracion < iter; iteracion++){
        for(int saIter = 0; saIter < iterTemp; saIter ++){
            std::vector<Paciente> nuevoAsignados = asignadosActual;
            std::vector<Paciente> nuevoNoAsignados = noAsignadosActual;
            std::vector<int> nuevaCapacidad = capacidadActual;
            std::sort(nuevoAsignados.begin(), nuevoAsignados.end(), sortAsignado());
            randomNumber = distribucion(generador);
            for(int i = 0; i < randomNumber; i++){
                randomProb = prob(generador);
                if(randomProb < paramProb){
                    std::shuffle(nuevoAsignados.end()-(nuevoAsignados.size()/2),nuevoAsignados.end(), generador);
                }
                Paciente eliminado = nuevoAsignados.back();
                //std::cout<<"Eliminado: "<< eliminado.id << " "<< eliminado.inicio << " " << eliminado.sesiones << "\n";
                recalculador(nuevaCapacidad, eliminado);
                nuevoNoAsignados.push_back(eliminado);
                nuevoAsignados.pop_back();
            }
            std::sort(nuevoNoAsignados.begin(), nuevoNoAsignados.end(), sortNoAsignado());
            randomProb = prob(generador);

                //usar ASAP para insertar.
            for(unsigned int j = 0; j < nuevoNoAsignados.size()/4;j++){
                std::uniform_int_distribution<int> salvavidas(0, nuevoNoAsignados.size() - 1);
                randomNumber = salvavidas(generador);
                Paciente porAsignar = nuevoNoAsignados[randomNumber];
                nuevoNoAsignados.erase(nuevoNoAsignados.begin()+randomNumber);
                //std::cout<<"Por asignar: " << porAsignar.release << " " << porAsignar.sesiones << "\n";
                ASAP(porAsignar, nuevaCapacidad, nuevoAsignados, nuevoNoAsignados);
            }
            fitNuevo = funcionObjetivo(nuevoAsignados);
            delta = solucionActual - fitNuevo;
            //std::cout << delta << "\n";

            //std::cout <<"Exponencial: " << exp(delta/temp)<< "Delta: " << delta << "Temperatura: " << temp << "\n";

            //std::cout << "Nuevo asignados: "<<nuevoNoAsignados.size() << " cantidadMinima " << cantidadMinima<< "\n";
            //Si solución nueva es mejor que la actual se actualiza
            if(delta > 0 and nuevoAsignados.size() >= cantidadMinima){
                //std::cout << "Entré\n";
                solucionActual = fitNuevo;
                asignadosActual = nuevoAsignados;
                noAsignadosActual = nuevoNoAsignados;
                capacidadActual = nuevaCapacidad;
            }
            //Generar número para ver si se acepta algo peor//

            else if(prob(generador) < exp(delta/temp)){
                solucionActual = fitNuevo;
                asignadosActual = nuevoAsignados;
                noAsignadosActual = nuevoNoAsignados;
                capacidadActual = nuevaCapacidad;
            }
            //Verificar si la solución actual es mejor que la mejor
            if(solucionActual < mejorSolucion and asignadosActual.size() >= cantidadMinima){
                //std::cout << "Encontré algo mejor\n";
                mejorSolucion = solucionActual;
                asignados = asignadosActual;
                noAsignados = noAsignadosActual;
                capacidadMaquinas = capacidadActual;
            }
            //std::cout << mejorSolucion << "\n";
        }
        temp = temp * multi;
        if(temp < 0.00001){
            //std::cout << "RECALENTAR!\n";
            temp = 1;
            solucionActual = mejorSolucion;
            asignadosActual = asignados;
            noAsignadosActual = noAsignados;
            capacidadActual = capacidadMaquinas;
        }
        //std::cout << iteracion << " " << mejorSolucion/(100*asignados.size()) << " " << asignados.size() << " \n";
    }
}

void Scheduler::printSolucion(){
    int counter = 0;
    for(auto &i:asignados){
        std::cout << i.id <<":  ";
        for(unsigned int j = 0; j < i.schedulePaciente.size();j++){
            counter++;
            std::cout << i.schedulePaciente[j];
            if(counter % 7 == 0){
                std::cout << " ";
            }
        }
        std::cout << " \n";
        counter = 0;
    }
    for(auto &i:noAsignados){
        std::cout << i.id <<":  ";
        for(unsigned int j = 0; j < i.schedulePaciente.size();j++){
            counter++;
            std::cout << i.schedulePaciente[j];
            if(counter % 7 == 0){
                std::cout << " ";
            }
        }
        std::cout << " \n";
        counter = 0;
    }
    std::cout << "\n";
    std::cout << "\n" << "Capacidades Máquinas \n";
    counter = 0;
    for(auto &i: capacidadMaquinas){
        if(counter % 7 == 0){
            std::cout << "\n";
        }
        if(counter == dias){
            counter = 0;
            std::cout << "\n  Máquina tipo 2 \n";
        }
        std::cout << i << " ";
        counter++;
    }
    std::cout << "\n";
}

void Scheduler::debugger(){
    for(auto &i: asignados){
        std::cout << i.release << " " << i.inicio <<" "<<i.tiempoEspera << "\n";
    }
}
