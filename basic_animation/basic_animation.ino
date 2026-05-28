/*
 * basic_animation.ino
 * Obra cinética: 16 antenas retráctiles en 4 anillos concéntricos (1 + 3 + 5 + 7).
 *
 * Secuencia: subida anillo a anillo (centro → fuera), pausa común arriba, bajada simultánea.
 * High Level Trigger: HIGH en INx = relé activado = antena sube.
 */

const int DELAY_CIRCULO_MS = 3000;
const int PAUSA_ARRIBA_MS  = 3000;

// true = HIGH activa relé | false = Low Level Trigger (invertir)
const bool HIGH_LEVEL_TRIGGER = true;

// Arduino D2..D13, A0..A3 → relés IN1..IN16
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

void setup() {
  for (int i = 0; i < 16; i++) {
    pinMode(PIN_ANTENAS[i], OUTPUT);
    escribirRele(i, false);
  }
}

void loop() {
  for (int c = 0; c < NUM_CIRCULOS; c++) {
    setCirculo(c, true);
    delay(DELAY_CIRCULO_MS);
  }

  delay(PAUSA_ARRIBA_MS);

  apagarTodas();

  delay(PAUSA_ARRIBA_MS);
}
