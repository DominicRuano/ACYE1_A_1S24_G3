#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEV 4
#define DATA 10
#define CS 9
#define CLK 8

String mensaje = ":v";
int velocidad = map(200, 1023, 0, 400, 15);
int valorBinarioSW1 = digitalRead(13);
int valorBinarioSW2 = digitalRead(12);
MD_Parola MATRICES = MD_Parola(HARDWARE_TYPE, DATA, CLK, CS, MAX_DEV);

int valor = 1;

//lista de animaciones
  const uint8_t animaciones[] = {
    PA_PRINT,
    PA_SCROLL_UP,
    PA_SLICE,
    PA_SCAN_HORIZ,
    PA_OPENING_CURSOR,
    PA_SCROLL_DOWN_RIGHT,
    PA_WIPE, 
    PA_GROW_UP, 
    PA_CLOSING_CURSOR,
    PA_SCROLL_UP_LEFT,
    PA_MESH,
    PA_OPENING,
    PA_SCROLL_UP_RIGHT, 
    PA_BLINDS,
    PA_DISSOLVE,
    PA_CLOSING, 
    PA_RANDOM, 
    PA_WIPE_CURSOR,
    PA_GROW_DOWN, 
    PA_SCAN_VERT, 
    PA_SCROLL_DOWN_LEFT
  };

uint8_t indiceAnimacion = random(sizeof(animaciones));
uint8_t animacionSeleccionada = animaciones[indiceAnimacion];

void PrintInMatrix(String mensaje){
  int SW1 = digitalRead(13);
  int SW2 = digitalRead(12);

  do {
    int valorPotenciometro = analogRead(A0);
    int valorMapeado = map(valorPotenciometro, 0, 1023, 0, 100);
    MATRICES.setSpeed(valorMapeado);

    if(valorBinarioSW2 == LOW){
      if(valorBinarioSW1 == LOW){
        MATRICES.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }else if(valorBinarioSW1 == HIGH){
        MATRICES.setTextEffect(PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
      }
    }else{
      MATRICES.setTextEffect(animacionSeleccionada, animacionSeleccionada);
    }

    valorBinarioSW1 = digitalRead(13);
    valorBinarioSW2 = digitalRead(12);
    if(MATRICES.displayAnimate()){
      indiceAnimacion = random(sizeof(animaciones));
      animacionSeleccionada = animaciones[indiceAnimacion];
      MATRICES.displayClear();
      MATRICES.displayReset();
      MATRICES.displayText(mensaje.c_str(), PA_LEFT, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      //Serial.Print("Esperando 30 seg, para continuar");
      //delay(30000);
      return;
    }

    if(SW1 != valorBinarioSW1 || SW2 != valorBinarioSW2 ){
      Serial.println("");
      Serial.println("Se detecto cambio de animacion");
      MATRICES.displayClear();
      delay(1000);
      PrintInMatrix(mensaje);
      return;
    }
  } while(true);
}

void setup() {
  //inicia comunicacion serial
  Serial.begin(9600);

  pinMode(13, INPUT);
  pinMode(12, INPUT);

  //inicia parola
  MATRICES.begin();
  MATRICES.setIntensity(5);
  MATRICES.displayClear();
  MATRICES.displayText(mensaje.c_str(), PA_LEFT, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  // Siempre debe estar en setup una llamada a la funcion PrintInMatrix con la dacena "" para inicializar.
  PrintInMatrix("");
  Serial.println("Matrices inicializadas con exito.");

  delay(1000);

  PrintInMatrix("Esto es una prueba");
  Serial.println("FIN");
}

void loop() {
  //String texto = "Hola mundo" + String(valor);
  //PrintInMatrix(texto);
  //valor = valor + 1;
  //Serial.println("termino la funcion");
}