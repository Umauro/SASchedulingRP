#ifndef PACIENTE_H
#define PACIENTE_H



class Paciente{
public:
    int id;
    int categoria;
    int release;
    int due;
    int sesiones;
    int interrupciones;
    int tipoMaquina;
    int tiempoPrimeraSesion;
    int tiempoSesion;
    int sesionesRestantes;
    std::vector<int> schedulePaciente;
    int tiempoEspera;
    int inicio;
    int fin;

    Paciente(int ide, int categor, int rel, int du, int ses,
            int interrup, int tipoMaquina, int tiempoS, int dias);

};


#endif
