# Sistemas Embebidos: Laboratorio 1
**Autor: Josmar Dominguez 16-10315**

En esta ocasión se centra el objetivo del laboratorio en trajar principalmente con listas (`queues`) lineales y circulares. Para ello se hace alusión de un sistema embebido compuesto por un microcontrolador y un acelerómetro que pueden interactuar mediante el protocolo de comunicación `SPI`, enviando registros con lectura y escritura, y enviando pulsos eléctricos para activar o notificar de interrupciones.


# Parte 1.1 Definición de pines

## Enunciado

En el sistema embebido descrito se está trabajando para realizar un dispositivo que sea capaz de medir la aceleración en tiempo real. Ergo se hace uso de un microcontrolador de 8 bits (PIC 18F47Q10) y un acelerómetro que se encuentra conectado a través de la interfaz `SPI` (Serial Peripherical Interface). La defición de los pines se presenta a continuación:

**Pines GPIO**
| GPIO 1           | GPIO 2 | GPIO 3 | GPIO 4 | GPIO 5 |
|------------------|--------|--------|--------|--------|
| Chip Select (CS) | INT 1  | INT 2  | INT 3  | INT 4  |
| `0XA0`           | `0XA1` | `0XA2` | `0XA3` | `0XA4` |

**Pines SPI**
| SDO                | SDI               | SCK          |
|--------------------|-------------------|--------------|
| Serial Data Output | Serial Data Input | Serial Clock |
| `0XE0`             | `0XE1`            | `0XE2`       |

## Solución
Se creó una enumeración para dirigir cada uno de los pines en valor hexadecimal al nombre, para hacer referencia a éstos por su nombre y facilitar la escritura y lectura del código.
Para los pines GPIO se creó la enumeración `enum GPIO`, y para los pines SPI se creó la enumeración `enum SPI`.

# Parte 1.2 Registros

## Enunciado
El dispositivo conectado cuenta con una lista de registros de **lectura** (R) además de **lectura y escritura** (R/W) que se usarán en la implementación final. Los mismos se encuentran a continuación:

**Register `0xFF` - Device Definitions (R)**
| Bit 7-4  | Bit 3        | Bit 2                   | Bit 1-0                 |
|----------|--------------|-------------------------|-------------------------|
| Always 0 | Device State | Extra Interrupts Enable | Gravity size definition |
* **Device State**: `0` significa estado apagado, `1` significa estado activo.	
* **Extra Interrupts Enable**: `0` significa que el dispositivo no las soprta, `1` significa que las puede usar
* **Gravity size definition**: Indica el tamaño necesario para mostrar la data en la consola.
    * `00` - 8 bits
    * `01` - 10 bits
    * `10` - 12 bits
    * `11` - 14 bits

**Register `0x10` Gravity_L (R)**
| Bit 7-2                   | Bit 1-0  |
|---------------------------|----------|
| Lower bits of measurement | Always 0 |

**Register `0x11` Gravity_H (R)**
| Bit 7-0                    |
|----------------------------|
| Higher bits of measurement |

**Register 0x20 - Interupt Configure (R/W)**
| Bit 7-4  | Bit 3             | Bit 2             | Bit 1                 | Bit 0      |
|----------|-------------------|-------------------|-----------------------|------------|
| Always 0 | Double Tap Enable | Inactivity Enable | Gravity Change Enable | Tap Enable |

Para cada uno de estos registros se deberán generar los `bitfields` correspondientes, así como las uniones necesarias para poder acceder al byte completo. Se debe poder generar un ítem para enviar en una cola que sirva para escritura o lectura de un registro.

## Solución
Para hacer la definición de los registros se creó la enumeración `enum REGISTERS` que dirige el valor hexadecimal del registro a un nombre descriptivo para facilitar su escritura.

Luego, se creó una unión para cada registro, de manera que en el campo `value` de la unión se guarda el byte completo, y luego en una estructura interna se acceden mediante `bitfields` a cadda uno de los segmentos.
* `typedef union deviceDefinition`
* `typedef union gravityHigh`
* `typedef union gravityLow`
* `typedef union interruptConfigure`

Luego, se creó una estructura genérica `data` para poder almacenar el registro en un ítem con los demás valores de interés: pin y registro.
```
typedef union data {
    uint8_t value;
    deviceDefinition device;
    gravityHigh gravityHigh;
    gravityLow gravityLow;
    interruptConfigure interrupt;
} data;
```

Con esto se puede crear el ítem que almacena la información completa del registro, pin usado y los valores del registro.
```
typedef struct Item {
    uint8_t pin;                // Pin to use
    enum REGISTERS reg;         // Register to use
    data data;                  // Data to send/receive
} Item;
```

Para el ítem se creó además, una función `create_item` para asignar el pin usado, la dirección del registro y los valores del registro.

# Parte 1.3 Interrupciones

## Enunciado
El acelerómetro conectado cuenta además con las siguientes interrupciones:
* Interrupción por toque (dar un toque a la placa del PCB)
* Interrupción al cambiar el centro de gravedad (Girar el PCB de su eje -z a otro eje)
* Interrupción por inactividad
* Interrupción por doble toque

Considerano cada una de estas interrupciones y los pines a los cuales se encuentran vinculados, genere un tipo de estructura que sirva como ítem de una cola.
| INT 1 | INT 2                        | INT 3       | INT 4     |
|-------|------------------------------|-------------|-----------|
| Toque | Cambio de centro de gravedad | Inactividad | Doble tap |

## Solución
Se creó la estructura `Interruption` con los campos `pin` y `description` que almacenan la información de la interrupción.

Además, una función `create_interruption` para que en función del pin recibido, se asigne la descripción correspondiente y cree la estructura `Interruption` correspondiente.

# Parte 2 Generación de cola para envío de datos e interrupciones

## Enunciado
Para este apartado se hará la implementación de una **cola lineal** y de una **circular**. 
* En la cola lineal se debe poder anexar lecturas y escrituras de los diversos registros con la estructura que usted definiese. Permitiend escribir y leer información de la cola.
* En la cola circular se debe permitir la recepción de distintos tipos de interrupciones, permitiendo manejar la situación de forma adecuada. En este caso, se debe desplegar en consola los diversos eventos que ocurran basados en la interrupción recibida.

## Solución
Se crearon las librerías `queue_linear.h` y `queue_circular.h` para la implementación de las colas lineales y circulares respectivamente.

Ya que ambas deben convivir en el mismo `main.c` -en una se debe almacenar los registros, en otra las interrupciones- se crearon estructuras y funciones con nombres descriptivos de cada tipo de cola. Además, para mejorar la legibilidad de ambas colas, se escribieron las funciones en `snake_case`, usando el prefijo `circular_` o `linear_` en cada caso.

Ambas colas tienen funciones de `push`, `pop`, `is_empty` y `is_full` para manejar los elementos de la cola. Y se definió un `Node` para cada una, para almacenar datos genéricos (pueden ser registros, interrupciones, o lo que sea) usando aputandores a vacío para poder almacenar cualquier tipo de dato.

# Conclusión y comentarios
Las colas son importantes para poder manejar eventos en el orden en el que se generan o disparan, pues sigue una lógica FIFO (First In, First Out), lo que es ideal para manejar registros e interrupciones en la comunicacióne entre un períferico o sensor y un microcontrolador.

Además, para el caso de las interrupciones es ideal manejarlas con una cola circular, y quizá modificar el comportamiento de ésta, de manera que se puda leer de manera continua e indefinida la lista de interrupciones hasta que se vayan procesando y eliminando de la cola.

Se quiso hacer una implementación de las funcionalidades de las colas (`push`, `pop`, etc.) como métodos dentro de la estructura, y así poder aplicar polimorfismo en el código, pero al tener que implementar ambas colas en el mismo archivo, se optó por hacer funciones independientes para cada cola, y así no aumentar la complejidad del código.