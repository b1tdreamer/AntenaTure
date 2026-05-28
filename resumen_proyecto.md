Resumen del Proyecto: Mecanismo de Antenas Retráctiles Controlado por Arduino
🎯 Objetivo Artístico
Crear una obra cinética que utilice múltiples antenas de coche retráctiles (de 8 a 30 unidades) que se desplieguen y recojan de forma controlada, creando un movimiento escultórico o una coreografía mecánica.

🧠 Sistema de Control
Componente	Especificación	Notas
Cerebro	Arduino Uno (o clon)	Se alimenta por USB (5V), no por la fuente de 12V
Actuador	Antena eléctrica retráctil de 12V	Cable rojo (+12V fijo), negro (GND), verde (señal de control)
Interruptor	Módulo de relés (16 canales, 12V, High Level Trigger)	Conecta/desconecta los 12V al cable verde de cada antena
Alimentación	Fuente de 12V / 30A (360W)	Industrial conmutada (tipo caja metálica con ventilador)
🔌 Esquema de Conexiones Eléctricas
text
[Fuente 12V 30A] ──cable 14 AWG──→ [Módulo relés VCC y COM]
                             │
                             └──cable 14 AWG──→ [Cable rojo de cada antena (12V fijo)]
                             
[Fuente GND] ──cable 14 AWG──→ [Módulo relés GND] ──→ [Cable negro de cada antena]

[Arduino pin Dx] ──→ [Módulo relés INx] (control de cada canal)

[Módulo relés NOx] ──cable 18-20 AWG──→ [Cable verde de cada antena]
Principio de funcionamiento:

El Arduino envía una señal HIGH (5V) al relé → el relé conecta COM con NO → los 12V llegan al cable verde → la antena sube.

El Arduino envía LOW (0V) → el relé se abre → se corta el 12V del cable verde → la antena baja por sí sola.

📊 Dimensionamiento Eléctrico
Concepto	Valor por antena	Para 8 antenas	Para 15-20 antenas
Consumo en pico (arranque)	2-3A	16-24A	30-60A
Consumo continuo	~0.5-1A	4-8A	7.5-15A
Módulo relés (16 canales)	~0.32A (todos activos)	0.32A	0.32A
Fuente recomendada	12V 3A	12V 30A	12V 50-60A (o varias de 20A)
Cableado fuente → relés: Para 8 antenas con fuente de 30A, usa cable 14 AWG (2.5 mm²) o 12 AWG para mayor margen.

🎛️ Estrategia para Escalar sin Sobrecargar la Fuente
Clave: No arrancar todas las antenas a la vez.

Con una fuente de 30A puedes alimentar hasta 30 antenas si activas los arranques de forma escalonada (ej. una antena cada segundo). El código sería:

cpp
// Arranque escalonado: 30 antenas, una cada segundo
for (int i = 0; i < 30; i++) {
  digitalWrite(pinRelay[i], HIGH);  // Sube antena i
  delay(1000);                      // Espera 1 segundo
}
Patrón de activación	Nº máximo de antenas con fuente de 30A
Arrancan todas a la vez	10-15
Arrancan en grupos de 3	20-25
Arrancan una detrás de otra	30 o más
🧾 Presupuesto Base (Prototipo de 1 antena)
Concepto	Coste
Antena retráctil 12V	25 €
Arduino Uno (clon)	18 €
Módulo relé 1 canal (5V)	4 €
Fuente 12V 3A	10 €
Cables, protoboard, caja	15 €
Total material	72 €
Mano de obra (12 h × 30€/h)	360 €
TOTAL + IVA	~500 €
*(Para escalar a 8 o más antenas, multiplica el coste de antenas y relés, y actualiza la fuente a ~40-60€)*

✅ Lista de Compras Recomendada (8 antenas)
Producto	Especificación	Notas
Antenas (8 uds)	12V eléctrica retráctil	Buscar en AliExpress, Amazon o desguace
Fuente alimentación	12V / 30A (360W)	Industrial conmutada. Búscala en AliExpress o Amazon
Módulo relés	16 canales, 12V, High Level Trigger	En AliExpress
Arduino Uno	Clon compatible	En Amazon o AliExpress
Cable 14 AWG	2.5 mm², rojo y negro	Para fuente → relés
Cable 18 AWG	0.75 mm², varios colores	Para relé → antenas
Bornes / regletas	Para distribución de 12V y GND	Opcional, pero muy útil
⚠️ Puntos Críticos a Recordar
No alimentes el Arduino desde la fuente de 12V. Usa USB o un cargador de móvil de 5V.

El módulo de relés debe ser de 12V para alimentarlo directamente con la fuente grande.

Compra "High Level Trigger" en el módulo de relés (se activa con HIGH del Arduino).

Escalona los arranques en el código para no sobrecargar la fuente.

No uses cable 16 AWG para la conexión principal (se quemaría). Usa 14 AWG o 12 AWG.

La antena necesita 12V fijo en el cable rojo (siempre conectado a la fuente). El relé solo conmuta el cable verde.

🚀 Posibles Evoluciones del Proyecto
Sustituir antenas por solenoides: Para golpear objetos y crear una batería robótica (movimiento instantáneo y contundente).

Añadir sensores: Para que las antenas reaccionen a sonido, luz o presencia.

Sincronización con música: Usar la librería de Arduino para que los movimientos sigan un ritmo.

¿Necesitas que desarrolle alguno de estos puntos con más detalle (código completo, esquemas de conexión, o un presupuesto formal para el cliente con 8 antenas)?