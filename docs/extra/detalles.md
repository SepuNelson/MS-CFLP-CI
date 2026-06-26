# Detalles Proyecto IA

## Datos Proyecto

Problema: Capacitated Facility Location Problem with Customer Incompatibilities (CFLP-CI)
Variantes: MS-CFLP-CI
Ayudante: Nangel Coello  
Correo Ayudante: nangel.coello@usm.cl

## Consideraciones

- Se considera que distancias, costo de combustible y demases se ven resumidos en un solo valor de costo de operación.

## Restricciones

- Cada cliente debe tener su demanda satisfecha completamente
- Cada bodega solo puede opere hasta como mucho su capacidad (no se pueden aceptar más clientes si se estaría pasando)
- Incopatibilidades: Una solución no puede contener ambos clientes de una incopatibilidad siendo servidos por la misma bodega. 

## Objetivo

Minimizar el costo total que genera la solución: suma de costos de apertura y de operación

## Datos de entrada

Los datos de entrada son por archivos de texto plano (.in) con la siguiente estructura:

```
<NW>
<NS>
<C_1> <C_2> <...>  <C_NW>
<FC_1> <FC_2> <...>  <FC_NW>
<G_1> <G_2> <...>  <G_NS>
<SC_1_1> <SC_1_2> <...> <SC_1_NW>
<SC_2_1> <SC_2_2> <...> <SC_2_NW>
.
.
.
<SC_NS_1> <SC_1_2> <...> <SC_NS_NW>
<NI>
<IP_1_1> <IP_1_2>
<IP_2_1> <IP_2_2>
.
.
.
<IP_NI_1> <IP_NI_2>
```

### Donde:
- $NW$ (int): numero de bodegas, numeradas de $1$ a $NW$
- $NS$ (int): numero de clientes, numerados de $1$ a $NS$
- $C\_i$ (int): capacidad de la bodega $i$
- $FC\_i$ (int): costo fijo de apertura de la bodega $i$
- $G\_j$ (int): demanda de el cliente $j$
- $SC\_j\_i$ (int): costo de operacion ***unitaro*** de satisfacer la demanda de $j$ desde la bodega $i$
- $NI$ (int): numero de incopatibilidades
- $IP\_j\_1/2$ (int): id de los clientes involucrados en una incopatibilidad.


## Formato salida:

La salida debe ser en un archivo de texto plano (.out) con formato:

```
<FO> <T> <S> <IT>
<Cl> <BD> <Ca>
.
.
.
<Cl> <BD> <Ca>
```

### Donde:
- $FO$: Valor de la funcion objetivo que da la solución
- $T$: Tiempo de ejecucion
- $S$: Semilla utilizada 
- $IT$: Numero de iteraciones para metodos de resolucion en los que sea relevante.
- $Cl$: id del cliente
- $BD$: id de la bodega
- $Ca$: Cantidad de productos que la bodega $BD$ esta enviando al cliente $Cl$


