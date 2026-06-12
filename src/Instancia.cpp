#include "Instancia.hpp"

#include <numeric>

using namespace std;

namespace {
long long sumar_vector(const vector<int>& valores) {
    return accumulate(valores.begin(), valores.end(), 0LL);
}
}

long long Instancia::capacidad_total() const {
    return sumar_vector(capacidad_bodega);
}

long long Instancia::demanda_total() const {
    return sumar_vector(demanda_cliente);
}

bool Instancia::dimensiones_validas() const {
    if (cantidad_bodegas <= 0 || cantidad_clientes <= 0) {
        return false;
    }

    if (static_cast<int>(capacidad_bodega.size()) != cantidad_bodegas ||
        static_cast<int>(costo_apertura.size()) != cantidad_bodegas ||
        static_cast<int>(demanda_cliente.size()) != cantidad_clientes ||
        static_cast<int>(costo_operacion.size()) != cantidad_clientes ||
        static_cast<int>(clientes_incompatibles.size()) != cantidad_clientes) {
        return false;
    }

    for (const auto& fila_costos : costo_operacion) {
        if (static_cast<int>(fila_costos.size()) != cantidad_bodegas) {
            return false;
        }
    }

    return true;
}
