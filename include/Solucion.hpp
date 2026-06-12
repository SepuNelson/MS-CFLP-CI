#pragma once

#include "Instancia.hpp"

#include <vector>

using namespace std;

struct Asignacion {
    int cliente = 0;
    int bodega = 0;
    int cantidad = 0;
};

struct Solucion {
    // envios[cliente][bodega] indica la cantidad enviada.
    vector<vector<int>> envios;
    vector<int> demanda_satisfecha;
    vector<int> capacidad_usada;
    vector<bool> bodega_abierta;
};

Solucion crear_solucion_vacia(const Instancia& instancia);
void actualizar_resumen_solucion(const Instancia& instancia, Solucion& solucion);
vector<Asignacion> obtener_asignaciones(const Solucion& solucion);
