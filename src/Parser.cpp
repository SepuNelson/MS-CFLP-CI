#include "Parser.hpp"

#include <fstream>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
int leer_entero(std::ifstream& archivo, const std::string& campo) {
    long long valor = 0;
    if (!(archivo >> valor)) {
        throw std::runtime_error("No se pudo leer el campo: " + campo);
    }
    if (valor < 0 || valor > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Valor fuera de rango en el campo: " + campo);
    }
    return static_cast<int>(valor);
}

int leer_entero_positivo(std::ifstream& archivo, const std::string& campo) {
    int valor = leer_entero(archivo, campo);
    if (valor <= 0) {
        throw std::runtime_error("El campo debe ser positivo: " + campo);
    }
    return valor;
}
}

Instancia leer_instancia(const std::string& ruta_archivo) {
    std::ifstream archivo(ruta_archivo);
    if (!archivo) {
        throw std::runtime_error("No se pudo abrir la instancia: " + ruta_archivo);
    }

    Instancia instancia;
    instancia.cantidad_bodegas = leer_entero_positivo(archivo, "cantidad_bodegas");
    instancia.cantidad_clientes = leer_entero_positivo(archivo, "cantidad_clientes");

    instancia.capacidad_bodega.resize(instancia.cantidad_bodegas);
    instancia.costo_apertura.resize(instancia.cantidad_bodegas);
    instancia.demanda_cliente.resize(instancia.cantidad_clientes);
    instancia.costo_operacion.assign(instancia.cantidad_clientes, std::vector<int>(instancia.cantidad_bodegas, 0));
    instancia.clientes_incompatibles.assign(instancia.cantidad_clientes, {});

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        instancia.capacidad_bodega[bodega] = leer_entero_positivo(archivo, "capacidad_bodega");
    }

    for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
        instancia.costo_apertura[bodega] = leer_entero(archivo, "costo_apertura");
    }

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        instancia.demanda_cliente[cliente] = leer_entero_positivo(archivo, "demanda_cliente");
    }

    for (int cliente = 0; cliente < instancia.cantidad_clientes; ++cliente) {
        for (int bodega = 0; bodega < instancia.cantidad_bodegas; ++bodega) {
            instancia.costo_operacion[cliente][bodega] = leer_entero(archivo, "costo_operacion");
        }
    }

    int cantidad_incompatibilidades =
        leer_entero(archivo, "cantidad_incompatibilidades");
    std::set<std::pair<int, int>> pares_unicos;

    for (int indice = 0; indice < cantidad_incompatibilidades; ++indice) {
        int cliente_uno = leer_entero_positivo(archivo, "cliente_incompatible_1") - 1;
        int cliente_dos = leer_entero_positivo(archivo, "cliente_incompatible_2") - 1;

        if (cliente_uno < 0 || cliente_uno >= instancia.cantidad_clientes || cliente_dos < 0 || cliente_dos >= instancia.cantidad_clientes) {
            throw std::runtime_error("Incompatibilidad con cliente fuera de rango");
        }

        if (cliente_uno == cliente_dos) {
            throw std::runtime_error("Un cliente no puede ser incompatible consigo mismo");
        }

        if (cliente_dos < cliente_uno) {
            std::swap(cliente_uno, cliente_dos);
        }

        if (pares_unicos.insert({cliente_uno, cliente_dos}).second) {
            instancia.incompatibilidades.push_back({cliente_uno, cliente_dos});
            instancia.clientes_incompatibles[cliente_uno].push_back(cliente_dos);
            instancia.clientes_incompatibles[cliente_dos].push_back(cliente_uno);
        }
    }

    std::string token_extra;
    if (archivo >> token_extra) {
        throw std::runtime_error("La instancia contiene datos extra al final");
    }

    if (!instancia.dimensiones_validas()) {
        throw std::runtime_error("La instancia tiene dimensiones inconsistentes");
    }

    return instancia;
}
