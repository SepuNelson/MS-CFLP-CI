#include "Evaluacion.hpp"
#include "Inicializador.hpp"
#include "Parser.hpp"
#include "Restricciones.hpp"
#include "Solucion.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void mostrar_uso(const char* programa) {
    std::cerr << "Uso: " << programa << " <ruta_instancia>\n";
}
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        mostrar_uso(argv[0]);
        return 1;
    }

    try {
        const std::string ruta_instancia = argv[1];
        auto tiempo_inicio = std::chrono::steady_clock::now();

        Instancia instancia = leer_instancia(ruta_instancia);
        Solucion solucion = generar_solucion_inicial(instancia);
        ResultadoFactibilidad factibilidad = verificar_factibilidad(instancia, solucion);

        if (!factibilidad.factible) {
            throw std::runtime_error(factibilidad.mensaje);
        }

        DesgloseCosto costo = evaluar_solucion(instancia, solucion);
        auto tiempo_fin = std::chrono::steady_clock::now();
        std::chrono::duration<double> tiempo = tiempo_fin - tiempo_inicio;

        const int semilla = 0;
        const int iteraciones = 0;

        std::cout << costo.costo_total << ' '
                  << std::fixed << std::setprecision(6) << tiempo.count() << ' '
                  << semilla << ' '
                  << iteraciones << '\n';

        for (const Asignacion& asignacion : obtener_asignaciones(solucion)) {
            std::cout << asignacion.cliente + 1 << ' '
                      << asignacion.bodega + 1 << ' '
                      << asignacion.cantidad << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
