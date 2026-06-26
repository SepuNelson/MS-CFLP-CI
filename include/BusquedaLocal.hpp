#pragma once

#include "Instancia.hpp"
#include "Solucion.hpp"

struct ParametrosBusqueda {
    unsigned int semilla = 0;
    double epsilon = 0.20;
    int reinicios = 10;
    int max_iteraciones_por_reinicio = 100;
};

struct ResultadoBusqueda {
    Solucion mejor_solucion;
    long long mejor_costo = 0;
    int iteraciones = 0;
    int reinicios_ejecutados = 0;
};

ResultadoBusqueda resolver_hill_climbing_restart(const Instancia& instancia, const ParametrosBusqueda& parametros);
