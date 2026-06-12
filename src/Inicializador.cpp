#include "Inicializador.hpp"

#include "Restricciones.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
struct CandidatoCliente {
    int cliente = -1;
    int cantidad_bodegas_factibles = 0;
    int grado_incompatibilidad = 0;
    int demanda = 0;
};

struct CandidatoBodega {
    int bodega = -1;
    long long costo_incremental = 0;
    int capacidad_restante_luego = 0;
};

bool puede_asignar_cliente(
    const Instancia& instancia,
    int cliente,
    int bodega,
    const std::vector<int>& capacidad_restante,
    const std::vector<int>& bodega_cliente
) {
    if (capacidad_restante[bodega] < instancia.demanda_cliente[cliente]) {
        return false;
    }

    for (int cliente_incompatible : instancia.clientes_incompatibles[cliente]) {
        if (bodega_cliente[cliente_incompatible] == bodega) {
            return false;
        }
    }

    return true;
}

int contar_bodegas_factibles(
    const Instancia& instancia,
    int cliente,
    const std::vector<int>& capacidad_restante,
    const std::vector<int>& bodega_cliente
) {
    int cantidad = 0;

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        if (puede_asignar_cliente(
                instancia,
                cliente,
                bodega,
                capacidad_restante,
                bodega_cliente
            )) {
            ++cantidad;
        }
    }

    return cantidad;
}

bool es_mejor_cliente(
    const CandidatoCliente& candidato,
    const CandidatoCliente& mejor
) {
    if (mejor.cliente == -1) {
        return true;
    }
    if (candidato.cantidad_bodegas_factibles != mejor.cantidad_bodegas_factibles) {
        return candidato.cantidad_bodegas_factibles < mejor.cantidad_bodegas_factibles;
    }
    if (candidato.grado_incompatibilidad != mejor.grado_incompatibilidad) {
        return candidato.grado_incompatibilidad > mejor.grado_incompatibilidad;
    }
    if (candidato.demanda != mejor.demanda) {
        return candidato.demanda > mejor.demanda;
    }
    return candidato.cliente < mejor.cliente;
}

int seleccionar_cliente_greedy(
    const Instancia& instancia,
    const std::vector<bool>& cliente_asignado,
    const std::vector<int>& capacidad_restante,
    const std::vector<int>& bodega_cliente
) {
    CandidatoCliente mejor;

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        if (cliente_asignado[cliente]) {
            continue;
        }

        CandidatoCliente candidato;
        candidato.cliente = cliente;
        candidato.cantidad_bodegas_factibles = contar_bodegas_factibles(instancia, cliente, capacidad_restante, bodega_cliente);
        candidato.grado_incompatibilidad = static_cast<int>(instancia.clientes_incompatibles[cliente].size());
        candidato.demanda = instancia.demanda_cliente[cliente];

        if (candidato.cantidad_bodegas_factibles == 0) {
            throw std::runtime_error(
                "No existe bodega factible para el cliente " +
                std::to_string(cliente + 1)
            );
        }

        if (es_mejor_cliente(candidato, mejor)) {
            mejor = candidato;
        }
    }

    return mejor.cliente;
}

bool es_mejor_bodega(const CandidatoBodega& candidato, const CandidatoBodega& mejor) {
    if (mejor.bodega == -1) {
        return true;
    }
    if (candidato.costo_incremental != mejor.costo_incremental) {
        return candidato.costo_incremental < mejor.costo_incremental;
    }
    if (candidato.capacidad_restante_luego != mejor.capacidad_restante_luego) {
        return candidato.capacidad_restante_luego > mejor.capacidad_restante_luego;
    }
    return candidato.bodega < mejor.bodega;
}

int seleccionar_bodega_greedy(
    const Instancia& instancia,
    int cliente,
    const std::vector<int>& capacidad_restante,
    const std::vector<int>& bodega_cliente,
    const std::vector<bool>& bodega_abierta
) {
    CandidatoBodega mejor;

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        if (!puede_asignar_cliente(instancia, cliente, bodega, capacidad_restante, bodega_cliente)) {
            continue;
        }

        CandidatoBodega candidato;
        candidato.bodega = bodega;
        candidato.costo_incremental = 1LL * instancia.demanda_cliente[cliente] * instancia.costo_operacion[cliente][bodega];

        if (!bodega_abierta[bodega]) {
            candidato.costo_incremental += instancia.costo_apertura[bodega];
        }

        candidato.capacidad_restante_luego = capacidad_restante[bodega] - instancia.demanda_cliente[cliente];

        if (es_mejor_bodega(candidato, mejor)) {
            mejor = candidato;
        }
    }

    return mejor.bodega;
}
}

Solucion generar_solucion_inicial_greedy(const Instancia& instancia) {
    if (!instancia.dimensiones_validas()) {
        throw std::runtime_error("No se puede inicializar una instancia invalida");
    }

    if (instancia.capacidad_total() < instancia.demanda_total()) {
        throw std::runtime_error("La capacidad total no alcanza para cubrir la demanda total");
    }

    Solucion solucion = crear_solucion_vacia(instancia);
    std::vector<int> capacidad_restante = instancia.capacidad_bodega;
    std::vector<int> bodega_cliente(instancia.cantidad_clientes, -1);
    std::vector<bool> bodega_abierta(instancia.cantidad_bodegas, false);
    std::vector<bool> cliente_asignado(instancia.cantidad_clientes, false);

    for (int paso = 0; paso < instancia.cantidad_clientes; ++paso) {
        int cliente = seleccionar_cliente_greedy(instancia, cliente_asignado, capacidad_restante, bodega_cliente);
        int mejor_bodega = seleccionar_bodega_greedy(instancia, cliente, capacidad_restante, bodega_cliente, bodega_abierta);

        if (mejor_bodega == -1) {
            throw std::runtime_error(
                "No se encontro bodega factible para el cliente " +
                std::to_string(cliente + 1)
            );
        }

        solucion.envios[cliente][mejor_bodega] = instancia.demanda_cliente[cliente];
        capacidad_restante[mejor_bodega] -= instancia.demanda_cliente[cliente];
        bodega_cliente[cliente] = mejor_bodega;
        bodega_abierta[mejor_bodega] = true;
        cliente_asignado[cliente] = true;
    }

    actualizar_resumen_solucion(instancia, solucion);
    ResultadoFactibilidad resultado = verificar_factibilidad(instancia, solucion);
    if (!resultado.factible) {
        throw std::runtime_error("La solucion inicial no es factible: " + resultado.mensaje);
    }

    return solucion;
}

Solucion generar_solucion_inicial(const Instancia& instancia) {
    return generar_solucion_inicial_greedy(instancia);
}
