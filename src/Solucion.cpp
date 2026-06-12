#include "Solucion.hpp"

#include <stdexcept>

Solucion crear_solucion_vacia(const Instancia& instancia) {
    Solucion solucion;
    solucion.envios.assign(
        instancia.cantidad_clientes,
        std::vector<int>(instancia.cantidad_bodegas, 0)
    );
    solucion.demanda_satisfecha.assign(instancia.cantidad_clientes, 0);
    solucion.capacidad_usada.assign(instancia.cantidad_bodegas, 0);
    solucion.bodega_abierta.assign(instancia.cantidad_bodegas, false);
    return solucion;
}

void actualizar_resumen_solucion(const Instancia& instancia, Solucion& solucion) {
    if (static_cast<int>(solucion.envios.size()) != instancia.cantidad_clientes) {
        throw std::runtime_error("La solucion no coincide con la cantidad de clientes");
    }

    solucion.demanda_satisfecha.assign(instancia.cantidad_clientes, 0);
    solucion.capacidad_usada.assign(instancia.cantidad_bodegas, 0);
    solucion.bodega_abierta.assign(instancia.cantidad_bodegas, false);

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        if (static_cast<int>(solucion.envios[cliente].size()) != instancia.cantidad_bodegas) {
            throw std::runtime_error("La solucion no coincide con la cantidad de bodegas");
        }

        for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
            int cantidad = solucion.envios[cliente][bodega];
            if (cantidad < 0) {
                throw std::runtime_error("La solucion contiene envios negativos");
            }

            solucion.demanda_satisfecha[cliente] += cantidad;
            solucion.capacidad_usada[bodega] += cantidad;
            if (cantidad > 0) {
                solucion.bodega_abierta[bodega] = true;
            }
        }
    }
}

std::vector<Asignacion> obtener_asignaciones(const Solucion& solucion) {
    std::vector<Asignacion> asignaciones;

    for (int cliente = 0; cliente < static_cast<int>(solucion.envios.size()); ++cliente) {
        for (int bodega = 0; bodega < static_cast<int>(solucion.envios[cliente].size()); ++bodega) {
            int cantidad = solucion.envios[cliente][bodega];
            if (cantidad > 0) {
                asignaciones.push_back({cliente, bodega, cantidad});
            }
        }
    }

    return asignaciones;
}
