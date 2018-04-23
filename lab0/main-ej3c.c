#include <stdio.h>

int main()
{
    int array [50];
    int i;

    /*
    Al inicializar el array el programa reserva el intervalo 
    de memoria necesario para guardar los elementos del array del tipo dado.
    Al intentar insertar un elemento en un indice mayor al maximo indice
    del arreglo, se intenta escribir fuera de la memoria reservada, generando un error.
    */
    for (i = 0; i < 51; i++){ 
        array[i] = i;
    }

    return 0;
}