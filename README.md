# INF295 - Inteligencia Artificial

Proyecto "Multi-Source Capacitated Facility Location Problem with Customer Incompatibilities" para la asigantura INF295 - Inteligencia Artificial, desarrollado en lenguaje C++

## Implementación Entrega 1

- Lectura de instancias `.in`.
- Representacion de solucion mediante matriz `envios[cliente][bodega]`.
- Verificacion de demanda, capacidad e incompatibilidades.
- Evaluacion de funcion objetivo: costo de apertura + costo operacional unitario por cantidad enviada.
- Generacion greedy de una solucion inicial factible.

La solucion inicial se construye con una regla greedy. En cada paso se selecciona el cliente no asignado mas restringido, es decir, el que tiene menos bodegas factibles disponibles. Luego se asigna completo a la bodega factible con menor costo incremental, considerando costo operacional y costo de apertura si la bodega aun no esta abierta. Esto sigue siendo una solucion valida para MS-CFLP-CI, mientras que la representacion mediante matriz de envios permite extender luego a clientes servidos por varias bodegas.

## Ubicación Local
En ubuntu ejecutar los siguientes comandos

```bash
win
cd Downloads/MS-CFLP-CI
```

## Compilar

```bash
make
```

## Ejecutar
Cambiar el toy.in por cualquier otra instancia

```bash
./bin/ms_cflp_ci data/toy.in > salida.out
```
