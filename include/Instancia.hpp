#pragma once

#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Instancia {
    int cantidad_bodegas = 0;
    int cantidad_clientes = 0;

    vector<int> capacidad_bodega;
    vector<int> costo_apertura;
    vector<int> demanda_cliente;

    // costo_operacion[cliente][bodega] es unitario.
    vector<vector<int>> costo_operacion;

    // Pares de clientes incompatibles usando indices internos desde 0.
    vector<pair<int, int>> incompatibilidades;
    vector<vector<int>> clientes_incompatibles;

    long long capacidad_total() const;
    long long demanda_total() const;
    bool dimensiones_validas() const;
};
