#pragma once

#include "Instancia.hpp"
#include "Solucion.hpp"

#include <string>

struct ResultadoFactibilidad {
    bool factible = false;
    std::string mensaje;
};

bool cumple_dimensiones(const Instancia& instancia, const Solucion& solucion);
bool cumple_demanda(const Instancia& instancia, const Solucion& solucion);
bool cumple_capacidad(const Instancia& instancia, const Solucion& solucion);
bool cumple_incompatibilidades(const Instancia& instancia, const Solucion& solucion);

ResultadoFactibilidad verificar_factibilidad(const Instancia& instancia, const Solucion& solucion);
