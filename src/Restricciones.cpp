#include "Restricciones.hpp"

bool cumple_dimensiones(const Instancia& instancia, const Solucion& solucion) {
    if (static_cast<int>(solucion.envios.size()) != instancia.cantidad_clientes) {
        return false;
    }

    for (const auto& envios_cliente : solucion.envios) {
        if (static_cast<int>(envios_cliente.size()) != instancia.cantidad_bodegas) {
            return false;
        }
    }

    return true;
}

bool cumple_demanda(const Instancia& instancia, const Solucion& solucion) {
    if (!cumple_dimensiones(instancia, solucion)) {
        return false;
    }

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        long long demanda_satisfecha = 0;
        for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
            if (solucion.envios[cliente][bodega] < 0) {
                return false;
            }
            demanda_satisfecha += solucion.envios[cliente][bodega];
        }

        if (demanda_satisfecha != instancia.demanda_cliente[cliente]) {
            return false;
        }
    }

    return true;
}

bool cumple_capacidad(const Instancia& instancia, const Solucion& solucion) {
    if (!cumple_dimensiones(instancia, solucion)) {
        return false;
    }

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        long long capacidad_usada = 0;
        for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
            if (solucion.envios[cliente][bodega] < 0) {
                return false;
            }
            capacidad_usada += solucion.envios[cliente][bodega];
        }

        if (capacidad_usada > instancia.capacidad_bodega[bodega]) {
            return false;
        }
    }

    return true;
}

bool cumple_incompatibilidades(const Instancia& instancia, const Solucion& solucion) {
    if (!cumple_dimensiones(instancia, solucion)) {
        return false;
    }

    for (const auto& incompatibilidad : instancia.incompatibilidades) {
        int cliente_uno = incompatibilidad.first;
        int cliente_dos = incompatibilidad.second;

        for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
            if (solucion.envios[cliente_uno][bodega] > 0 && solucion.envios[cliente_dos][bodega] > 0) {
                return false;
            }
        }
    }

    return true;
}

ResultadoFactibilidad verificar_factibilidad(const Instancia& instancia, const Solucion& solucion) {
    if (!cumple_dimensiones(instancia, solucion)) {
        return {false, "Dimensiones invalidas"};
    }
    if (!cumple_demanda(instancia, solucion)) {
        return {false, "No se satisface exactamente la demanda"}; 
    }
    if (!cumple_capacidad(instancia, solucion)) {
        return {false, "Se supera la capacidad de alguna bodega"};
    }
    if (!cumple_incompatibilidades(instancia, solucion)) {
        return {false, "Hay clientes incompatibles servidos por la misma bodega"};
    }

    return {true, "Solucion factible"};
}
