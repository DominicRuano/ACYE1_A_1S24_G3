#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Matrices
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEV 4
#define DATA 10
#define CS 9
#define CLK 8

// variables para nicializar las matrices
String mensaje = ":v";
int velocidad = map(200, 1023, 0, 400, 15);
int valorBinarioSW1 = digitalRead(13);
int valorBinarioSW2 = digitalRead(12);
MD_Parola MATRICES = MD_Parola(HARDWARE_TYPE, DATA, CLK, CS, MAX_DEV);
int valor = 1;

//lista de animaciones parola
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

// Funcio que mostrar en las matrices el String que se le envie.
void PrintInMatrix(String mensaje){
  int SW1 = digitalRead(13); // lee los pines 13 y 12 para el cambio de direccion y animacion
  int SW2 = digitalRead(12);

  do {
    int valorPotenciometro = analogRead(A0); // lee el valor de potenciometro
    int valorMapeado = map(valorPotenciometro, 0, 1023, 0, 100); // mapea el valor del potenciometro
    MATRICES.setSpeed(valorMapeado); // Define la velocidad de la animacion en tiempo de ejecucion

    // Define el modo de animacion segun los estados de los switchs
    if(valorBinarioSW2 == LOW){
      if(valorBinarioSW1 == LOW){
        MATRICES.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }else if(valorBinarioSW1 == HIGH){
        MATRICES.setTextEffect(PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
      }
    }else{
      MATRICES.setTextEffect(animacionSeleccionada, animacionSeleccionada);
    }

    // mide los valores de los pines 12 y 13 que son los switchs de animaciones 
    valorBinarioSW1 = digitalRead(13);
    valorBinarioSW2 = digitalRead(12);
    
    //termina la animacion
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

    //Detecta un cambio en los estados de los switches de animacion para volver a mostrar la animacion
    if(SW1 != valorBinarioSW1 || SW2 != valorBinarioSW2 ){
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

  //inicializa los pines 12 y 13 como de entrada
  pinMode(13, INPUT);
  pinMode(12, INPUT);

  //inicia parola
  MATRICES.begin();
  MATRICES.setIntensity(5);
  MATRICES.displayClear();
  MATRICES.displayText(mensaje.c_str(), PA_LEFT, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  // Siempre debe estar en setup una llamada a la funcion PrintInMatrix con la dacena "" para inicializar.
  PrintInMatrix("");

  //Ejemplo de como se debe llamar a la funcion PrintInMatrix para imprimir algo
  PrintInMatrix("PRUEBA");
}

void loop() {
  //codigo loop
}