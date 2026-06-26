#include "BusquedaLocal.hpp"
#include "Parser.hpp"
#include "Restricciones.hpp"
#include "Solucion.hpp"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using namespace std;

namespace {
constexpr double EPSILON_PREDETERMINADO = 0.20;
constexpr int REINICIOS_PREDETERMINADOS = 10;
constexpr int ITERACIONES_PREDETERMINADAS = 100;

void mostrar_uso(const char* programa) {
    cerr << "Uso: " << programa << " <ruta_instancia> [semilla] [epsilon] [reinicios] [iteraciones_por_reinicio]\n";
    cerr << "  epsilon debe estar entre 0 y 1. Valor por defecto: " << EPSILON_PREDETERMINADO << "\n";
    cerr << "  reinicios por defecto: " << REINICIOS_PREDETERMINADOS << "\n";
    cerr << "  iteraciones por reinicio por defecto: " << ITERACIONES_PREDETERMINADAS << "\n";
}

unsigned int generar_semilla() {
    auto ahora = static_cast<unsigned long long>(chrono::high_resolution_clock::now().time_since_epoch().count());
    return static_cast<unsigned int>(ahora ^ (ahora >> 32));
}

unsigned int leer_semilla(const string& texto) {
    if (texto.empty() || texto[0] == '-') {
        throw runtime_error("La semilla debe ser un entero sin signo valido");
    }

    size_t posicion = 0;
    unsigned long valor = stoul(texto, &posicion);
    if (posicion != texto.size() || valor > numeric_limits<unsigned int>::max()) {
        throw runtime_error("La semilla debe ser un entero sin signo valido");
    }
    return static_cast<unsigned int>(valor);
}

double leer_epsilon(const string& texto) {
    size_t posicion = 0;
    double valor = stod(texto, &posicion);
    if (posicion != texto.size() || !isfinite(valor) || valor < 0.0 || valor > 1.0) {
        throw runtime_error("epsilon debe estar entre 0 y 1");
    }
    return valor;
}

int leer_entero_positivo(const string& texto, const string& nombre) {
    if (texto.empty() || texto[0] == '-') {
        throw runtime_error(nombre + " debe ser un entero positivo");
    }

    size_t posicion = 0;
    long valor = stol(texto, &posicion);
    if (posicion != texto.size() || valor <= 0 || valor > numeric_limits<int>::max()) {
        throw runtime_error(nombre + " debe ser un entero positivo");
    }
    return static_cast<int>(valor);
}

int leer_entero_no_negativo(const string& texto, const string& nombre) {
    if (texto.empty() || texto[0] == '-') {
        throw runtime_error(nombre + " debe ser un entero no negativo");
    }

    size_t posicion = 0;
    long valor = stol(texto, &posicion);
    if (posicion != texto.size() || valor < 0 || valor > numeric_limits<int>::max()) {
        throw runtime_error(nombre + " debe ser un entero no negativo");
    }
    return static_cast<int>(valor);
}
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 6) {
        mostrar_uso(argv[0]);
        return 1;
    }

    try {
        const string ruta_instancia = argv[1];
        const unsigned int semilla = argc >= 3 ? leer_semilla(argv[2]) : generar_semilla();
        const double epsilon = argc >= 4 ? leer_epsilon(argv[3]) : EPSILON_PREDETERMINADO;
        const int reinicios = argc >= 5 ? leer_entero_positivo(argv[4], "reinicios") : REINICIOS_PREDETERMINADOS;
        const int iteraciones_por_reinicio = argc >= 6 ? leer_entero_no_negativo(argv[5], "iteraciones_por_reinicio") : ITERACIONES_PREDETERMINADAS;

        auto tiempo_inicio = chrono::steady_clock::now();

        Instancia instancia = leer_instancia(ruta_instancia);
        ParametrosBusqueda parametros;
        parametros.semilla = semilla;
        parametros.epsilon = epsilon;
        parametros.reinicios = reinicios;
        parametros.max_iteraciones_por_reinicio = iteraciones_por_reinicio;

        ResultadoBusqueda resultado = resolver_hill_climbing_restart(instancia, parametros);
        ResultadoFactibilidad factibilidad = verificar_factibilidad(instancia, resultado.mejor_solucion);

        if (!factibilidad.factible) {
            throw runtime_error(factibilidad.mensaje);
        }

        auto tiempo_fin = chrono::steady_clock::now();
        chrono::duration<double> tiempo = tiempo_fin - tiempo_inicio;

        cout << resultado.mejor_costo << ' ' << fixed << setprecision(6) << tiempo.count() << ' ' << semilla << ' ' << resultado.iteraciones << '\n';

        for (const Asignacion& asignacion : obtener_asignaciones(resultado.mejor_solucion)) {
            cout << asignacion.cliente + 1 << ' ' << asignacion.bodega + 1 << ' ' << asignacion.cantidad << '\n';
        }
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
