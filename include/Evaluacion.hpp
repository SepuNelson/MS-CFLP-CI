#pragma once

#include "Instancia.hpp"
#include "Solucion.hpp"

struct DesgloseCosto {
    long long costo_apertura = 0;
    long long costo_operacion = 0;
    long long costo_total = 0;
};

DesgloseCosto evaluar_solucion(const Instancia& instancia, const Solucion& solucion);
long long calcular_funcion_objetivo(const Instancia& instancia, const Solucion& solucion);
