#include "Inicializador.hpp"

#include "Restricciones.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace {
constexpr int INTENTOS_CONSTRUCCION_ALEATORIA = 50;

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
    const vector<int>& capacidad_restante,
    const vector<int>& bodega_cliente
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

bool usar_decision_aleatoria(mt19937& generador, double epsilon) {
    bernoulli_distribution distribucion(epsilon);
    return distribucion(generador);
}

int tamano_lista_restringida(int cantidad_candidatos) {
    return max(1, (cantidad_candidatos + 2) / 3);
}

int elegir_indice_aleatorio(mt19937& generador, int limite_exclusivo) {
    uniform_int_distribution<int> distribucion(0, limite_exclusivo - 1);
    return distribucion(generador);
}

int contar_bodegas_factibles( const Instancia& instancia, int cliente, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente) {
    int cantidad = 0;

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        if (puede_asignar_cliente( instancia, cliente, bodega, capacidad_restante, bodega_cliente )) {
            ++cantidad;
        }
    }

    return cantidad;
}

bool es_mejor_cliente( const CandidatoCliente& candidato, const CandidatoCliente& mejor) {
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

vector<CandidatoCliente> obtener_clientes_candidatos(const Instancia& instancia, const vector<bool>& cliente_asignado, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente) {
    vector<CandidatoCliente> candidatos;

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
            throw runtime_error(
                "No existe bodega factible para el cliente " +
                to_string(cliente + 1)
            );
        }

        candidatos.push_back(candidato);
    }

    sort(candidatos.begin(), candidatos.end(), [](const CandidatoCliente& izquierda, const CandidatoCliente& derecha) {
        return es_mejor_cliente(izquierda, derecha);
    });

    return candidatos;
}

int seleccionar_cliente_greedy(const Instancia& instancia, const vector<bool>& cliente_asignado, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente) {
    vector<CandidatoCliente> candidatos = obtener_clientes_candidatos(instancia, cliente_asignado, capacidad_restante, bodega_cliente);
    return candidatos.front().cliente;
}

int seleccionar_cliente_epsilon_greedy(const Instancia& instancia, const vector<bool>& cliente_asignado, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente, mt19937& generador, double epsilon) {
    vector<CandidatoCliente> candidatos = obtener_clientes_candidatos(instancia, cliente_asignado, capacidad_restante, bodega_cliente);

    if (!usar_decision_aleatoria(generador, epsilon)) {
        return candidatos.front().cliente;
    }

    int cantidad_elegible = tamano_lista_restringida(static_cast<int>(candidatos.size()));
    return candidatos[elegir_indice_aleatorio(generador, cantidad_elegible)].cliente;
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

vector<CandidatoBodega> obtener_bodegas_candidatas(const Instancia& instancia, int cliente, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente, const vector<bool>& bodega_abierta) {
    vector<CandidatoBodega> candidatos;

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

        candidatos.push_back(candidato);
    }

    sort(candidatos.begin(), candidatos.end(), [](const CandidatoBodega& izquierda, const CandidatoBodega& derecha) {
        return es_mejor_bodega(izquierda, derecha);
    });

    return candidatos;
}

int seleccionar_bodega_greedy(const Instancia& instancia, int cliente, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente, const vector<bool>& bodega_abierta) {
    vector<CandidatoBodega> candidatos = obtener_bodegas_candidatas(instancia, cliente, capacidad_restante, bodega_cliente, bodega_abierta);
    if (candidatos.empty()) {
        return -1;
    }
    return candidatos.front().bodega;
}

int seleccionar_bodega_epsilon_greedy(const Instancia& instancia, int cliente, const vector<int>& capacidad_restante, const vector<int>& bodega_cliente, const vector<bool>& bodega_abierta, mt19937& generador, double epsilon) {
    vector<CandidatoBodega> candidatos = obtener_bodegas_candidatas(instancia, cliente, capacidad_restante, bodega_cliente, bodega_abierta);
    if (candidatos.empty()) {
        return -1;
    }

    if (!usar_decision_aleatoria(generador, epsilon)) {
        return candidatos.front().bodega;
    }

    int cantidad_elegible = tamano_lista_restringida(static_cast<int>(candidatos.size()));
    return candidatos[elegir_indice_aleatorio(generador, cantidad_elegible)].bodega;
}

Solucion construir_solucion_inicial(const Instancia& instancia, mt19937* generador, double epsilon) {
    if (!instancia.dimensiones_validas()) {
        throw runtime_error("No se puede inicializar una instancia invalida");
    }

    if (instancia.capacidad_total() < instancia.demanda_total()) {
        throw runtime_error("La capacidad total no alcanza para cubrir la demanda total");
    }

    Solucion solucion = crear_solucion_vacia(instancia);
    vector<int> capacidad_restante = instancia.capacidad_bodega;
    vector<int> bodega_cliente(instancia.cantidad_clientes, -1);
    vector<bool> bodega_abierta(instancia.cantidad_bodegas, false);
    vector<bool> cliente_asignado(instancia.cantidad_clientes, false);

    for (int paso = 0; paso < instancia.cantidad_clientes; ++paso) {
        int cliente = -1;
        int mejor_bodega = -1;

        if (generador != nullptr && epsilon > 0.0) {
            cliente = seleccionar_cliente_epsilon_greedy(instancia, cliente_asignado, capacidad_restante, bodega_cliente, *generador, epsilon);
            mejor_bodega = seleccionar_bodega_epsilon_greedy(instancia, cliente, capacidad_restante, bodega_cliente, bodega_abierta, *generador, epsilon);
        } else {
            cliente = seleccionar_cliente_greedy(instancia, cliente_asignado, capacidad_restante, bodega_cliente);
            mejor_bodega = seleccionar_bodega_greedy(instancia, cliente, capacidad_restante, bodega_cliente, bodega_abierta);
        }

        if (mejor_bodega == -1) {
            throw runtime_error(
                "No se encontro bodega factible para el cliente " +
                to_string(cliente + 1)
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
        throw runtime_error("La solucion inicial no es factible: " + resultado.mensaje);
    }

    return solucion;
}
}

Solucion generar_solucion_inicial_greedy(const Instancia& instancia) {
    return construir_solucion_inicial(instancia, nullptr, 0.0);
}

Solucion generar_solucion_inicial_epsilon_greedy(const Instancia& instancia, unsigned int semilla, double epsilon) {
    if (epsilon < 0.0 || epsilon > 1.0) {
        throw runtime_error("El valor de epsilon debe estar entre 0 y 1");
    }

    if (epsilon == 0.0) {
        return generar_solucion_inicial_greedy(instancia);
    }

    mt19937 generador(semilla);

    for (int intento = 0; intento < INTENTOS_CONSTRUCCION_ALEATORIA; ++intento) {
        try {
            return construir_solucion_inicial(instancia, &generador, epsilon);
        } catch (const runtime_error&) {
            // Una eleccion aleatoria puede dejar sin candidatos a un cliente posterior.
        }
    }

    return generar_solucion_inicial_greedy(instancia);
}

Solucion generar_solucion_inicial(const Instancia& instancia) {
    return generar_solucion_inicial_greedy(instancia);
}
