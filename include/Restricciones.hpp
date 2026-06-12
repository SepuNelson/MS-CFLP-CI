#pragma once

#include "Instancia.hpp"
#include "Solucion.hpp"

#include <string>

using namespace std;

struct ResultadoFactibilidad {
    bool factible = false;
    string mensaje;
};

bool cumple_dimensiones(const Instancia& instancia, const Solucion& solucion);
bool cumple_demanda(const Instancia& instancia, const Solucion& solucion);
bool cumple_capacidad(const Instancia& instancia, const Solucion& solucion);
bool cumple_incompatibilidades(const Instancia& instancia, const Solucion& solucion);

ResultadoFactibilidad verificar_factibilidad(const Instancia& instancia, const Solucion& solucion);
