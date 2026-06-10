/*
 * multi_animation.ino
 * Rotador de animaciones para 16 antenas (1 + 3 + 5 + 7).
 * Cada animación dura ANIMATION_DURATION_MS; luego pasa a la siguiente.
 *
 * Todas las animaciones empiezan en el centro y avanzan hacia fuera.
 * Solo una antena activa a la vez (máx. ~0,7 A).
 */

const unsigned long ANIMATION_DURATION_MS = 60000UL;
const int DELAY_CIRCULO_MS = 3000;
const int DELAY_RAPIDO_MS    = 1500;

const bool HIGH_LEVEL_TRIGGER = true;

const int PIN_ANTENAS[16] = {
  2,
  3, 4, 5,
  6, 7, 8, 9, 10,
  11, 12, 13, A0, A1, A2, A3
};

// Orden centro → fuera (índices 0..15)
const int ORDEN_CENTRO_FUERA[] = {
  0,
  1, 2, 3,
  4, 5, 6, 7, 8,
  9, 10, 11, 12, 13, 14, 15
};
const int NUM_ANTENAS = 16;

// Anillos impares (0, 1 y 3) en orden centro → fuera
const int ORDEN_ANILLOS_IMPARES[] = {
  0,
  1, 2, 3,
  9, 10, 11, 12, 13, 14, 15
};
const int NUM_ANILLOS_IMPARES = 11;

// --- Hardware ---

void escribirRele(int indice, bool activo) {
  bool nivel = HIGH_LEVEL_TRIGGER ? activo : !activo;
  digitalWrite(PIN_ANTENAS[indice], nivel ? HIGH : LOW);
}

void apagarTodas() {
  for (int i = 0; i < NUM_ANTENAS; i++) {
    escribirRele(i, false);
  }
}

// --- Motor de animaciones ---

struct Animacion {
  const char* nombre;
  void (*reset)();
  void (*tick)(unsigned long ahora);
};

void esperarHasta(unsigned long ahora, unsigned long& proximoEvento, unsigned long duracionMs) {
  proximoEvento = ahora + duracionMs;
}

// Avanza una antena a la vez por una secuencia centro → fuera
struct OlaAntena {
  const int* secuencia;
  int tamSecuencia;
  int paso;
  int intervaloMs;
  bool reiniciarAlFinal;
  unsigned long proximoEvento;

  void reset(const int* seq, int tam, int intervalo, bool reiniciar) {
    apagarTodas();
    secuencia = seq;
    tamSecuencia = tam;
    intervaloMs = intervalo;
    reiniciarAlFinal = reiniciar;
    paso = 0;
    escribirRele(secuencia[paso], true);
    proximoEvento = millis() + intervaloMs;
  }

  void tick(unsigned long ahora) {
    if (ahora < proximoEvento) {
      return;
    }

    if (paso < 0) {
      paso = 0;
      escribirRele(secuencia[paso], true);
      esperarHasta(ahora, proximoEvento, intervaloMs);
      return;
    }

    escribirRele(secuencia[paso], false);
    paso++;

    if (paso >= tamSecuencia) {
      if (reiniciarAlFinal) {
        paso = -1;
        esperarHasta(ahora, proximoEvento, intervaloMs);
        return;
      }
      paso = 0;
    }

    escribirRele(secuencia[paso], true);
    esperarHasta(ahora, proximoEvento, intervaloMs);
  }
};

OlaAntena olaCentroFuera;
OlaAntena olaReinicio;
OlaAntena olaRapida;
OlaAntena olaAnillosImpares;
OlaAntena olaUnaAntena;

// Animación 0: ola continua centro → fuera, una antena a la vez
namespace Anim0 {
  void reset() {
    olaCentroFuera.reset(ORDEN_CENTRO_FUERA, NUM_ANTENAS, DELAY_CIRCULO_MS, false);
  }

  void tick(unsigned long ahora) {
    olaCentroFuera.tick(ahora);
  }
}

// Animación 1: expansión centro → fuera y reinicio (todas abajo antes de repetir)
namespace Anim1 {
  void reset() {
    olaReinicio.reset(ORDEN_CENTRO_FUERA, NUM_ANTENAS, DELAY_CIRCULO_MS, true);
  }

  void tick(unsigned long ahora) {
    olaReinicio.tick(ahora);
  }
}

// Animación 2: misma ola pero más rápida
namespace Anim2 {
  void reset() {
    olaRapida.reset(ORDEN_CENTRO_FUERA, NUM_ANTENAS, DELAY_RAPIDO_MS, false);
  }

  void tick(unsigned long ahora) {
    olaRapida.tick(ahora);
  }
}

// Animación 3: anillos impares (0, 1 y 3), una antena a la vez
namespace Anim3 {
  void reset() {
    olaAnillosImpares.reset(ORDEN_ANILLOS_IMPARES, NUM_ANILLOS_IMPARES, DELAY_CIRCULO_MS, false);
  }

  void tick(unsigned long ahora) {
    olaAnillosImpares.tick(ahora);
  }
}

// Animación 4: recorre todas las antenas en orden centro → fuera
namespace Anim4 {
  void reset() {
    olaUnaAntena.reset(ORDEN_CENTRO_FUERA, NUM_ANTENAS, DELAY_CIRCULO_MS, false);
  }

  void tick(unsigned long ahora) {
    olaUnaAntena.tick(ahora);
  }
}

void anim0_reset() { Anim0::reset(); }
void anim0_tick(unsigned long ahora) { Anim0::tick(ahora); }
void anim1_reset() { Anim1::reset(); }
void anim1_tick(unsigned long ahora) { Anim1::tick(ahora); }
void anim2_reset() { Anim2::reset(); }
void anim2_tick(unsigned long ahora) { Anim2::tick(ahora); }
void anim3_reset() { Anim3::reset(); }
void anim3_tick(unsigned long ahora) { Anim3::tick(ahora); }
void anim4_reset() { Anim4::reset(); }
void anim4_tick(unsigned long ahora) { Anim4::tick(ahora); }

const Animacion ANIMACIONES[] = {
  {"Ola centro-fuera",       anim0_reset, anim0_tick},
  {"Expansion y reinicio",   anim1_reset, anim1_tick},
  {"Ola rapida",             anim2_reset, anim2_tick},
  {"Ola anillos impares",    anim3_reset, anim3_tick},
  {"Una antena a la vez",    anim4_reset, anim4_tick},
};

const int NUM_ANIMACIONES = sizeof(ANIMACIONES) / sizeof(ANIMACIONES[0]);

int indiceAnimacion = 0;
unsigned long inicioAnimacionMs = 0;

void cambiarAnimacion(int nuevoIndice) {
  apagarTodas();
  indiceAnimacion = nuevoIndice;
  inicioAnimacionMs = millis();
  ANIMACIONES[indiceAnimacion].reset();
}

void setup() {
  for (int i = 0; i < NUM_ANTENAS; i++) {
    pinMode(PIN_ANTENAS[i], OUTPUT);
    escribirRele(i, false);
  }

  Serial.begin(9600);
  cambiarAnimacion(0);
  Serial.println(ANIMACIONES[0].nombre);
}

void loop() {
  unsigned long ahora = millis();

  if (ahora - inicioAnimacionMs >= ANIMATION_DURATION_MS) {
    int siguiente = (indiceAnimacion + 1) % NUM_ANIMACIONES;
    cambiarAnimacion(siguiente);
    Serial.println(ANIMACIONES[siguiente].nombre);
  }

  ANIMACIONES[indiceAnimacion].tick(ahora);
}
