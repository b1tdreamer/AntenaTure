# Antenas retráctiles — obra cinética con Arduino

Obra escultórica con **16 antenas eléctricas retráctiles** dispuestas en **4 anillos concéntricos** (1 + 3 + 5 + 7), controladas por **Arduino Uno** y un **módulo de 16 relés a 12 V**.

## Estructura de la instalación

Vista cenital (numeración lógica de antenas y relés):

```
              [15]  [16]
           [13]        [14]
        [11]      ·      [12]
           [9]   [1]   [10]
              [7] [8]
           [5]        [6]
              [3] [4]
                 [2]
```

| Anillo | Antenas | Relés (IN) | Pines Arduino |
|--------|---------|------------|---------------|
| 0 — Centro | 1 | IN1 | D2 |
| 1 | 3 | IN2–IN4 | D3, D4, D5 |
| 2 | 5 | IN5–IN9 | D6–D10 |
| 3 | 7 | IN10–IN16 | D11–D13, A0–A3 |

## Componentes

| Componente | Especificación |
|------------|----------------|
| Microcontrolador | Arduino Uno (o clon) — alimentación por **USB 5 V** |
| Relés | Módulo 16 canales, **12 V**, High Level Trigger |
| Antenas | Acero inoxidable, **12 V DC**, configuración **B** (control por cable verde) |
| Fuente | **12 V / 30 A** (industrial conmutada recomendada) |

### Antena (AliExpress)

Referencia: [kit antena retráctil 12 V](https://es.aliexpress.com/item/1005006603010083.html)

| Parámetro | Valor |
|-----------|-------|
| Voltaje | 12 V CC (rango 10–14 V) |
| Corriente | **0,7 ± 0,2 A** por antena |
| Extensión | ~780 mm |
| Longitud bajo guardabarros | 325 mm |
| Cable | 1200 mm (rojo, negro, verde) |
| Fuerza de elevación | > 4 kg |
| Vida útil | > 50 000 ciclos |
| Ruido | < 50 dB |

**Conexión de la antena (modo B — control externo):**

- **Rojo** → +12 V fijo (siempre conectado a la fuente)
- **Negro** → GND
- **Verde** → conmutado por el relé (12 V para subir; abierto para bajar)

> No uses la configuración A (verde unido directamente al +12 V): la antena subiría/bajaría sola al encender la fuente.

### Consumo eléctrico estimado

Con **0,7 A/antena** (dato del fabricante):

| Situación | Corriente aprox. |
|-----------|------------------|
| 1 antena | ~0,7 A |
| Anillo exterior (7 antenas) | ~4,9 A |
| 16 antenas activas | ~11 A |
| Módulo relés (todos ON) | ~0,3 A |

Una fuente de **30 A** es holgada para 16 unidades. La animación activa los anillos **de forma escalonada** (3 s entre cada uno) para suavizar picos y crear efecto de onda.

**Tiempo de extensión:** el fabricante no lo indica. Con ~780 mm de carrera, lo habitual es **5–8 s** de subida y **4–6 s** de bajada. Mídelo con cronómetro al montar la primera antena y ajusta `DELAY_CIRCULO_MS` en el código si quieres que cada anillo termine antes de activar el siguiente.

## Cableado

### Alimentación común

```
Fuente 12V (+) ──┬──→ VCC módulo relés
                 ├──→ COM de cada relé (bus +12V)
                 └──→ Cable ROJO de cada antena

Fuente 12V (−) ──┬──→ GND módulo relés
                 ├──→ Cable NEGRO de cada antena
                 └──→ GND Arduino

PC / cargador ──→ USB Arduino (5 V)
```

### Por cada antena (×16)

```
Relé N:
  COM → +12 V
  NO  → cable VERDE de la antena

Arduino pin → INN del módulo
```

### Arduino ↔ módulo de relés

| Arduino | Relé |
|---------|------|
| D2 | IN1 |
| D3 | IN2 |
| D4 | IN3 |
| D5 | IN4 |
| D6 | IN5 |
| D7 | IN6 |
| D8 | IN7 |
| D9 | IN8 |
| D10 | IN9 |
| D11 | IN10 |
| D12 | IN11 |
| D13 | IN12 |
| A0 | IN13 |
| A1 | IN14 |
| A2 | IN15 |
| A3 | IN16 |
| GND | GND |

### Reglas de seguridad

- **No** alimentes el Arduino desde la fuente de 12 V.
- **No** conectes 12 V a los pines IN del módulo (solo señales 5 V del Arduino).
- Usa **GND común** entre fuente, módulo relés y Arduino.
- Cable **14 AWG** (2,5 mm²) para fuente → regletas; **18–20 AWG** para relé → verdes.
- Un relé por antena (no agrupar varias antenas en un solo contacto).

## Programas incluidos

### `rele101/rele101.ino`

Prueba de un solo relé (pin D2 → IN1). Alterna ON/OFF cada 2 segundos.

### `basic_animation/basic_animation.ino`

Animación fija por anillos concéntricos:

1. **Subida:** centro → anillo 3 → anillo 5 → anillo 7 (3 s entre cada uno)
2. **Pausa** 3 s con todas arriba
3. **Bajada:** todas a la vez
4. **Pausa** 3 s y repetir

### `multi_animation/multi_animation.ino`

Rotador de **5 animaciones**. Cada una dura **1 minuto** y pasa automáticamente a la siguiente. **Nunca activa las 16 antenas a la vez** (máximo un anillo por paso, hasta 7 antenas). Abre el **Monitor Serie** (9600 baud) para ver cuál está activa.

| # | Nombre | Comportamiento |
|---|--------|----------------|
| 0 | Ola centro-fuera | Sube anillo 0 → a los 3 s sube anillo 1 y **baja** el 0 → continúa la ola |
| 1 | Ola fuera-centro | Igual, pero empieza por el anillo exterior |
| 2 | Ola rápida | Misma lógica, 1,5 s entre pasos |
| 3 | Ola anillos impares | Alterna solo anillos 1 y 3 (máx. 7 antenas) |
| 4 | Una antena a la vez | Recorre las 16 de una en una (~0,7 A) |

Constantes editables al inicio del sketch:

```cpp
const unsigned long ANIMATION_DURATION_MS = 60000UL;  // duración de cada animación
const int DELAY_CIRCULO_MS = 3000;   // tiempo que cada anillo permanece activo
const int DELAY_RAPIDO_MS    = 1500;  // intervalo de la ola rápida
const bool HIGH_LEVEL_TRIGGER = true;
```

Para añadir una animación nueva, copia un bloque `namespace AnimX`, regístralo en el array `ANIMACIONES[]` y aumenta `NUM_ANIMACIONES`.

Si el relé solo hace clic con LOW, cambia `HIGH_LEVEL_TRIGGER` a `false`.

## Carga del firmware

1. Instala [Arduino IDE](https://www.arduino.cc/en/software).
2. Conecta el Arduino por USB.
3. **Herramientas → Placa → Arduino Uno**.
4. **Herramientas → Puerto →** (el COM de tu placa).
5. Abre el sketch deseado y pulsa **Subir**.

Para probar sin antenas: escucha los clics de los relés o usa multímetro en continuidad entre COM y NO de cada canal.

## Prueba gradual recomendada

1. `rele101.ino` — un relé, sin antena.
2. Una antena en IN1 — comprobar subida/bajada manual.
3. `basic_animation.ino` — solo relés conectados, verificar secuencia por anillos.
4. Conectar antenas anillo a anillo hasta completar las 16.

## Estructura del repositorio

```
.
├── README.md                 ← este archivo
├── resumen_proyecto.md       ← notas de diseño y presupuesto
├── rele101/
│   └── rele101.ino           ← prueba 1 relé
├── basic_animation/
│   └── basic_animation.ino   ← animación fija 16 antenas
└── multi_animation/
    └── multi_animation.ino   ← rotador de 5 animaciones (1 min c/u)
```

## Evoluciones posibles

- Ajustar tiempos según medición real de extensión (~780 mm).
- Sincronización con música o sensores (sonido, luz, presencia).
- Patrones alternativos (ondas, aleatorio, reacción al público).
