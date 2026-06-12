#pragma once

#include <string>
#include <utility>
#include <vector>

struct Instancia {
    int cantidad_bodegas = 0;
    int cantidad_clientes = 0;

    std::vector<int> capacidad_bodega;
    std::vector<int> costo_apertura;
    std::vector<int> demanda_cliente;

    // costo_operacion[cliente][bodega] es unitario.
    std::vector<std::vector<int>> costo_operacion;

    // Pares de clientes incompatibles usando indices internos desde 0.
    std::vector<std::pair<int, int>> incompatibilidades;
    std::vector<std::vector<int>> clientes_incompatibles;

    long long capacidad_total() const;
    long long demanda_total() const;
    bool dimensiones_validas() const;
};
