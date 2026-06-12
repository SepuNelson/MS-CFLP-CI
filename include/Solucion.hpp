#pragma once

#include "Instancia.hpp"

#include <vector>

struct Asignacion {
    int cliente = 0;
    int bodega = 0;
    int cantidad = 0;
};

struct Solucion {
    // envios[cliente][bodega] indica la cantidad enviada.
    std::vector<std::vector<int>> envios;
    std::vector<int> demanda_satisfecha;
    std::vector<int> capacidad_usada;
    std::vector<bool> bodega_abierta;
};

Solucion crear_solucion_vacia(const Instancia& instancia);
void actualizar_resumen_solucion(const Instancia& instancia, Solucion& solucion);
std::vector<Asignacion> obtener_asignaciones(const Solucion& solucion);
