/*
 * multi_animation.ino
 * Rotador de animaciones para 16 antenas (1 + 3 + 5 + 7).
 * Cada animación dura ANIMATION_DURATION_MS; luego pasa a la siguiente.
 *
 * Nunca se activan todos los relés a la vez: como máximo un anillo sube
 * por paso (máx. 7 antenas en el anillo exterior).
 *
 * Animación 0: ola continua centro → fuera; cada 3 s sube el siguiente
 * anillo y baja el anterior (sin esperar a que estén todas arriba).
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

const int CIRCULO_0[] = {0};
const int CIRCULO_1[] = {1, 2, 3};
const int CIRCULO_2[] = {4, 5, 6, 7, 8};
const int CIRCULO_3[] = {9, 10, 11, 12, 13, 14, 15};

const int* CIRCULOS[]     = {CIRCULO_0, CIRCULO_1, CIRCULO_2, CIRCULO_3};
const int  TAM_CIRCULOS[] = {1, 3, 5, 7};
const int  NUM_CIRCULOS   = 4;

// --- Hardware ---

void escribirRele(int indice, bool activo) {
  bool nivel = HIGH_LEVEL_TRIGGER ? activo : !activo;
  digitalWrite(PIN_ANTENAS[indice], nivel ? HIGH : LOW);
}

void setCirculo(int circulo, bool subir) {
  const int* miembros = CIRCULOS[circulo];
  int n = TAM_CIRCULOS[circulo];
  for (int i = 0; i < n; i++) {
    escribirRele(miembros[i], subir);
  }
}

void apagarTodas() {
  for (int i = 0; i < 16; i++) {
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

int avanzarCirculo(int circulo, int direccion) {
  circulo += direccion;
  if (circulo >= NUM_CIRCULOS) {
    circulo = 0;
  }
  if (circulo < 0) {
    circulo = NUM_CIRCULOS - 1;
  }
  return circulo;
}

// Ola por anillos: sube el siguiente y baja el anterior cada DELAY_CIRCULO_MS
struct OlaAnillos {
  int circuloActivo;
  int direccion;
  int intervaloMs;
  unsigned long proximoEvento;

  void reset(int dir, int intervalo) {
    apagarTodas();
    direccion = dir;
    intervaloMs = intervalo;
    circuloActivo = (dir > 0) ? 0 : NUM_CIRCULOS - 1;
    setCirculo(circuloActivo, true);
    proximoEvento = millis() + intervaloMs;
  }

  void tick(unsigned long ahora) {
    if (ahora < proximoEvento) {
      return;
    }

    int anterior = circuloActivo;
    circuloActivo = avanzarCirculo(circuloActivo, direccion);

    setCirculo(anterior, false);
    setCirculo(circuloActivo, true);

    esperarHasta(ahora, proximoEvento, intervaloMs);
  }
};

OlaAnillos olaCentroFuera;
OlaAnillos olaFueraCentro;
OlaAnillos olaRapida;

// Animación 0: ola continua centro → fuera
namespace Anim0 {
  void reset() {
    olaCentroFuera.reset(1, DELAY_CIRCULO_MS);
  }

  void tick(unsigned long ahora) {
    olaCentroFuera.tick(ahora);
  }
}

// Animación 1: ola continua fuera → centro
namespace Anim1 {
  void reset() {
    olaFueraCentro.reset(-1, DELAY_CIRCULO_MS);
  }

  void tick(unsigned long ahora) {
    olaFueraCentro.tick(ahora);
  }
}

// Animación 2: misma ola pero más rápida
namespace Anim2 {
  void reset() {
    olaRapida.reset(1, DELAY_RAPIDO_MS);
  }

  void tick(unsigned long ahora) {
    olaRapida.tick(ahora);
  }
}

// Animación 3: ola solo por anillos impares (1 y 3), nunca más de 7 antenas
namespace Anim3 {
  const int ANILLOS[] = {1, 3};
  const int NUM_ANILLOS = 2;

  int indice;
  int circuloActivo;
  unsigned long proximoEvento;

  void reset() {
    apagarTodas();
    indice = 0;
    circuloActivo = ANILLOS[0];
    setCirculo(circuloActivo, true);
    proximoEvento = millis() + DELAY_CIRCULO_MS;
  }

  void tick(unsigned long ahora) {
    if (ahora < proximoEvento) {
      return;
    }

    int anterior = circuloActivo;
    indice = (indice + 1) % NUM_ANILLOS;
    circuloActivo = ANILLOS[indice];

    setCirculo(anterior, false);
    setCirculo(circuloActivo, true);

    esperarHasta(ahora, proximoEvento, DELAY_CIRCULO_MS);
  }
}

// Animación 4: una sola antena a la vez (máx. ~0,7 A)
namespace Anim4 {
  int indiceActivo;
  unsigned long proximoEvento;

  void reset() {
    apagarTodas();
    indiceActivo = 0;
    escribirRele(indiceActivo, true);
    proximoEvento = millis() + DELAY_CIRCULO_MS;
  }

  void tick(unsigned long ahora) {
    if (ahora < proximoEvento) {
      return;
    }

    escribirRele(indiceActivo, false);
    indiceActivo = (indiceActivo + 1) % 16;
    escribirRele(indiceActivo, true);

    esperarHasta(ahora, proximoEvento, DELAY_CIRCULO_MS);
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
  {"Ola fuera-centro",       anim1_reset, anim1_tick},
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
  for (int i = 0; i < 16; i++) {
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
