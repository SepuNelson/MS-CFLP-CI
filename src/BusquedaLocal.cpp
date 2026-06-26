#include "BusquedaLocal.hpp"

#include "Evaluacion.hpp"
#include "Inicializador.hpp"
#include "Restricciones.hpp"

#include <algorithm>
#include <limits>
#include <random>
#include <stdexcept>

using namespace std;

namespace {
struct Movimiento {
    int cliente = -1;
    int origen = -1;
    int destino = -1;
    int cantidad = 0;
    long long delta_costo = 0;
    bool existe = false;
};

struct ResultadoLocal {
    Solucion solucion;
    long long costo = 0;
    int iteraciones = 0;
};

bool destino_respeta_incompatibilidades(const Instancia& instancia, const Solucion& solucion, int cliente, int destino) {
    for (int cliente_incompatible : instancia.clientes_incompatibles[cliente]) {
        if (solucion.envios[cliente_incompatible][destino] > 0) {
            return false;
        }
    }

    return true;
}

bool es_mejor_movimiento(const Movimiento& candidato, const Movimiento& mejor) {
    if (!mejor.existe) {
        return true;
    }
    if (candidato.delta_costo != mejor.delta_costo) {
        return candidato.delta_costo < mejor.delta_costo;
    }
    if (candidato.cantidad != mejor.cantidad) {
        return candidato.cantidad > mejor.cantidad;
    }
    if (candidato.cliente != mejor.cliente) {
        return candidato.cliente < mejor.cliente;
    }
    if (candidato.origen != mejor.origen) {
        return candidato.origen < mejor.origen;
    }
    return candidato.destino < mejor.destino;
}

long long calcular_delta_movimiento(const Instancia& instancia, const Solucion& solucion, int cliente, int origen, int destino, int cantidad) {
    long long delta = 1LL * cantidad * (instancia.costo_operacion[cliente][destino] - instancia.costo_operacion[cliente][origen]);

    if (solucion.capacidad_usada[destino] == 0) {
        delta += instancia.costo_apertura[destino];
    }

    if (solucion.capacidad_usada[origen] == cantidad) {
        delta -= instancia.costo_apertura[origen];
    }

    return delta;
}

Movimiento buscar_mejor_movimiento(const Instancia& instancia, const Solucion& solucion) {
    Movimiento mejor;

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        for (int origen = 0; origen < instancia.cantidad_bodegas; ++origen) {
            int cantidad_origen = solucion.envios[cliente][origen];
            if (cantidad_origen <= 0) {
                continue;
            }

            for (int destino = 0; destino < instancia.cantidad_bodegas; ++destino) {
                if (destino == origen) {
                    continue;
                }

                int capacidad_disponible = instancia.capacidad_bodega[destino] - solucion.capacidad_usada[destino];
                if (capacidad_disponible <= 0) {
                    continue;
                }

                if (!destino_respeta_incompatibilidades(instancia, solucion, cliente, destino)) {
                    continue;
                }

                int cantidad = min(cantidad_origen, capacidad_disponible);
                long long delta = calcular_delta_movimiento(instancia, solucion, cliente, origen, destino, cantidad);
                if (delta >= 0) {
                    continue;
                }

                Movimiento candidato;
                candidato.cliente = cliente;
                candidato.origen = origen;
                candidato.destino = destino;
                candidato.cantidad = cantidad;
                candidato.delta_costo = delta;
                candidato.existe = true;

                if (es_mejor_movimiento(candidato, mejor)) {
                    mejor = candidato;
                }
            }
        }
    }

    return mejor;
}

void aplicar_movimiento(const Instancia& instancia, Solucion& solucion, const Movimiento& movimiento) {
    solucion.envios[movimiento.cliente][movimiento.origen] -= movimiento.cantidad;
    solucion.envios[movimiento.cliente][movimiento.destino] += movimiento.cantidad;

    solucion.capacidad_usada[movimiento.origen] -= movimiento.cantidad;
    solucion.capacidad_usada[movimiento.destino] += movimiento.cantidad;

    solucion.bodega_abierta[movimiento.origen] = solucion.capacidad_usada[movimiento.origen] > 0;
    solucion.bodega_abierta[movimiento.destino] = solucion.capacidad_usada[movimiento.destino] > 0;

    (void)instancia;
}

ResultadoLocal ejecutar_hill_climbing_best_improvement(const Instancia& instancia, Solucion solucion, int max_iteraciones) {
    ResultadoLocal resultado;
    resultado.solucion = solucion;
    resultado.costo = calcular_funcion_objetivo(instancia, resultado.solucion);

    for (int iteracion = 0; iteracion < max_iteraciones; ++iteracion) {
        ++resultado.iteraciones;
        Movimiento movimiento = buscar_mejor_movimiento(instancia, resultado.solucion);
        if (!movimiento.existe) {
            break;
        }

        aplicar_movimiento(instancia, resultado.solucion, movimiento);
        resultado.costo += movimiento.delta_costo;
    }

    ResultadoFactibilidad factibilidad = verificar_factibilidad(instancia, resultado.solucion);
    if (!factibilidad.factible) {
        throw runtime_error("La busqueda local genero una solucion infactible: " + factibilidad.mensaje);
    }

    return resultado;
}

void validar_parametros(const ParametrosBusqueda& parametros) {
    if (parametros.epsilon < 0.0 || parametros.epsilon > 1.0) {
        throw runtime_error("epsilon debe estar entre 0 y 1");
    }
    if (parametros.reinicios <= 0) {
        throw runtime_error("La cantidad de reinicios debe ser positiva");
    }
    if (parametros.max_iteraciones_por_reinicio < 0) {
        throw runtime_error("La cantidad maxima de iteraciones no puede ser negativa");
    }
}
}

ResultadoBusqueda resolver_hill_climbing_restart(const Instancia& instancia, const ParametrosBusqueda& parametros) {
    validar_parametros(parametros);

    mt19937 generador_semillas(parametros.semilla);
    uniform_int_distribution<unsigned int> distribuir_semilla(0, numeric_limits<unsigned int>::max());

    ResultadoBusqueda resultado;
    bool existe_mejor = false;

    for (int reinicio = 0; reinicio < parametros.reinicios; ++reinicio) {
        unsigned int semilla_reinicio = reinicio == 0 ? parametros.semilla : distribuir_semilla(generador_semillas);

        Solucion solucion_inicial = generar_solucion_inicial_epsilon_greedy(instancia, semilla_reinicio, parametros.epsilon);
        ResultadoLocal resultado_local = ejecutar_hill_climbing_best_improvement(
            instancia,
            solucion_inicial,
            parametros.max_iteraciones_por_reinicio
        );

        resultado.iteraciones += resultado_local.iteraciones;
        ++resultado.reinicios_ejecutados;

        if (!existe_mejor || resultado_local.costo < resultado.mejor_costo) {
            resultado.mejor_solucion = resultado_local.solucion;
            resultado.mejor_costo = resultado_local.costo;
            existe_mejor = true;
        }
    }

    return resultado;
}
