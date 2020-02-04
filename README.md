# SchedulingRP
Project for Advanced Artificial Intelligence (INF-388)

# Instrucciones de Compilación
Para compilar solo se debe utilizar el comando `make`.

# Instrucciones de Ejecución
Para ejecutar se debe utilizar el siguiente comando
~~~
./exec INSTANCIA=[ruta a la instancia] ITER=[Iteraciones SA] PARAM1=[int mayor a 1] PARAMPROB=[Float entre 0 y 1] TEMP=[Float, usar números bajos] MULTI=[Float entre 0 y 1] ITERTEMP=[Iteraciones con temperatura fija] GRASPITER=[Iteraciones Grasp Simplificado]
~~~
Al utilizar `make run` se ejecuta el algoritmo con los parámetros por defecto utilizando la instancia `CL_60_30_1.rad`.

Si se selección GRASPITER como 1 se ejecutará solo el Simulated Annealing desarrollado.
