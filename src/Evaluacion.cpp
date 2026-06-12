#include "Evaluacion.hpp"

#include "Restricciones.hpp"

#include <stdexcept>

using namespace std;

DesgloseCosto evaluar_solucion(const Instancia& instancia, const Solucion& solucion) {
    if (!cumple_dimensiones(instancia, solucion)) {
        throw runtime_error("No se puede evaluar una solucion con dimensiones invalidas");
    }

    DesgloseCosto desglose;
    vector<bool> bodega_utilizada(instancia.cantidad_bodegas, false);

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
            int cantidad = solucion.envios[cliente][bodega];
            if (cantidad < 0) {
                throw runtime_error("No se puede evaluar una solucion con envios negativos");
            }

            if (cantidad > 0) {
                bodega_utilizada[bodega] = true;
                desglose.costo_operacion += 1LL * cantidad * instancia.costo_operacion[cliente][bodega];
            }
        }
    }

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        if (bodega_utilizada[bodega]) {
            desglose.costo_apertura += instancia.costo_apertura[bodega];
        }
    }

    desglose.costo_total = desglose.costo_apertura + desglose.costo_operacion;
    return desglose;
}

long long calcular_funcion_objetivo(const Instancia& instancia, const Solucion& solucion) {
    return evaluar_solucion(instancia, solucion).costo_total;
}
