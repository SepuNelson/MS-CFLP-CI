# INF295 - Inteligencia Artificial

Proyecto en C++ para resolver instancias del problema **Multi-Source Capacitated Facility Location Problem with Customer Incompatibilities (MS-CFLP-CI)**.

El programa construye soluciones factibles con una inicializacion greedy estocastica epsilon-greedy y luego mejora cada solucion mediante **Hill Climbing Best Improvement + Restart**. La salida corresponde a la mejor solucion encontrada por la heuristica.

## Estructura esperada de la entrega

```text
MS-CFLP-CI/
|-- data/          # Instancias .in
|-- include/       # Cabeceras .hpp
|-- src/           # Implementacion .cpp
|-- main.cpp
|-- Makefile
`-- README.md
```

## Requisitos

El Makefile esta preparado para un entorno tipo Linux/Unix, (Yo usé Ubuntu), o una terminal compatible que tenga `make` y `g++`.

## Entrar a la carpeta del proyecto

Ubicarse en la carpeta donde se encuentra el proyecto.

-- Ubicación Personal

```bash
win
cd Downloads/MS-CFLP-CI
```

## Compilar

Desde la raiz del proyecto:

```bash
make
```

Para limpiar los archivos generados:

```bash
make clean
```

## Ejecutar

```bash
./bin/ms_cflp_ci <ruta_instancia> [semilla] [epsilon] [reinicios] [iteraciones_por_reinicio] > salida.out
```

Argumentos:

- `ruta_instancia`: archivo `.in` que se quiere resolver.
- `semilla`: entero sin signo usado para reproducir la corrida. Si no se indica, el programa genera una semilla automaticamente.
- `epsilon`: probabilidad de tomar una decision aleatoria en la construccion epsilon-greedy. Debe estar entre `0` y `1`. Valor por defecto: `0.20`.
- `reinicios`: cantidad de reinicios del Hill Climbing. Debe ser mayor que `0`. Valor por defecto: `10`.
- `iteraciones_por_reinicio`: maximo de iteraciones de busqueda local por reinicio. Debe ser mayor o igual que `0`. Valor por defecto: `100`.

Ejemplo:

```bash
./bin/ms_cflp_ci data/toy.in 12345 0.20 10 100 > salida.out
```

Ejecutar una construccion greedy deterministica, sin busqueda local (Lo usé para compararla con la anterior):

```bash
./bin/ms_cflp_ci data/toy.in 0 0 1 0 > salida.out
```
