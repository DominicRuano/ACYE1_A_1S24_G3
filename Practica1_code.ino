#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

//KEYPAD
#include <Key.h>
#include <Keypad.h>

// Matrices
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEV 4
#define DATA 10
#define CS 9
#define CLK 8

    //VARIABLES Y DECLARACIONES PARA KEYPAD Y SISTEMA DE SEGURIDAD

const byte Filas = 4; //tamaño de filas keypad
const byte Columnas = 4; //tamaño de columnas keypad

//LED DE ADVERTENCIA ERRORES EN CONTRASEÑA
const int ledPin1 = 53;
const int ledPin2 = 51;
const int ledPin3 = 49;

//SWITCH PARA VERIFICAR EL ESTADO DE CALCULADORA 
const int switchPin = 47;  // Define el pin al que está conectado el switch
int switchState = 0;      // Variable para almacenar el estado del switch
int lastSwitchState = 0;  // Variable para almacenar el estado anterior del switch

char Teclado[Filas][Columnas] = {

{'7','8','9','A'},
{'4','5','6','B'},
{'1','2','3','C'},
{'*','0','#','D'}
}; //Disposicion de las teclas del keypad

byte Fpines[Filas] = {22,23,25,27}; // pines de entrada del keypad para las filas
byte Cpines[Columnas] = {29,31,33,35}; // pines de entrada del keypad para las columnas

Keypad teclado = Keypad(makeKeymap(Teclado), Fpines, Cpines, Filas, Columnas); //asignacion del keupad 

String Entrada; //variable para capturar la contraseña ingresada

int Entradas = 0; //cantidad de veces entrando al sistema

int IntentosEntrada = 0; //Intentos entrar en el sistema fallidos

bool salida = true;

String Pasword = "AC6C124"; //CONTRASEÑA POR DEFECTO DEL SISTEMA

//

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

 //CODIGO FUNCION PARA SISTEMA DE SEGURIDAD

 void SistemaDeSeguridad(){

  char Ingreso = teclado.getKey(); // VARIABLE QUE CAPTURA LA ENTRADA DE CARACTERES POR MEDIO DEL KEYPAD

  if (Entradas == 0){ //MENSAJE DE BIENVENIDA AL USUARIO
    Serial.println("||----BIENVENIDO USUARIO DEL GRUPO 6----||");
    Serial.print("INGRESE LA CONTRASEA: ");
    Entradas +=1;
  }

  while (true){

    Ingreso = teclado.getKey();

    if (Ingreso){ //COMPROBACION DE QUE NO SE HA INGRESADO NADA DENTRO DE LA VARIABLE

    if (Ingreso == '#'){ //UTILIZANDO EL SIGNO * COMO ENTER PARA PODER INGRESAR CONTRASEÑA
      Serial.println();
      Serial.print("CONTRASEÑA INGRESADA: ");
      Serial.println(Entrada);
      
      if (Entrada == Pasword){
        Serial.println("Desbloqueado  "); // ESTADO 2
        Serial.println(" ");
        Serial.println("||---- BIENVENIDO!----||"); //SI LA CONTRASEÑA ES CORRECTA DA MENSAJE DE BIENVENIDA
        
        while (salida){

          char key = teclado.getKey();

          switchState = digitalRead(switchPin);  // Lee el estado actual del switch

            // Verifica si el estado del switch ha cambiado
            if (switchState != lastSwitchState) {
              // Si el switch ha sido presionado, Cambia el estado a calculadora
              if (switchState == HIGH) {

                //CONDIGO DE LA CALCULADORA HERE!!!!!!

              Serial.println("MODO CALCULADORA");

              }
              delay(50);  // Retardo pequeño para evitar rebotes del switch
            }

            // Almacena el estado actual del switch para la próxima iteración
            lastSwitchState = switchState;
          

          if (key == '*') { // cambio de contraseña
            Serial.print("INGRESE NUEVA CONTRASEÑA: ");

            Pasword = ""; // limpiar la nueva contraseña

            while (true) {
              key = teclado.getKey();
              Serial.println("Presione # para confirmar nueva contrasenia");
              if (key == '#') { //ACEPTAR NUEVA CONTRASEÑA
                
                if (Pasword.length() >= 6 && Pasword.length() <= 8) { //COMPROBACION TAMAÑO CORRECTO DE LA NUEVA CONTRASEÑA
                  Serial.println();
                  Serial.println("Nueva Contraseña: " + Pasword); //MOSTRAR CAMBIO DE CONTRASEÑA
                  Serial.println("Contrasenia actualizada");
                  break; //SALIR DEL BUCLE DE CREADO CONTRASEÑA
                } else {
                  Serial.println();
                  Serial.println("TAMAÑO DE LA CONTRASEÑA INCORRECTO!"); //AVISO DE CONTRASEÑA INCORRECTA
                  Serial.println("Longitud actual: " + String(Pasword.length())); //MOSTRAR TAMAÑO CONTRASEÑA
                  Serial.println("Contrasenia actualizada");
                }
              } else if (key != NO_KEY) {
                Pasword += key; // Solo concatenar si se presiona una tecla válida
                
                Serial.print(key);
              }
            }

            salida = false;
          }
          
        }    

      }else{
        Serial.println("CONTRASEÑA INGRESADA ES INCORRECTA!"); //AVISO CONTRASEÑA INCORRECTA
        IntentosEntrada +=1; //AUMENTO DE INTENTO ENTRADAS

        if (IntentosEntrada == 1){
          
          digitalWrite(ledPin1, HIGH);//encender led

        }else if (IntentosEntrada == 2){

          digitalWrite(ledPin2, HIGH);

        }else if (IntentosEntrada == 3){

          digitalWrite(ledPin3, HIGH);

          Serial.println("Máximo de Intentos Fallidos... "); //ESTADO 03
          Serial.println(" ");
          Serial.println("NIÑO MALO, TE TOCA ESPERAR 15 SEGUNDOS >:) ");

          delay(15000); //TIEMPO ENSEPERA DE MALAL CONTRASEÑA
          
          digitalWrite(ledPin1, LOW); //APAGAR EL LED
          digitalWrite(ledPin2, LOW);
          digitalWrite(ledPin3, LOW);

          IntentosEntrada = 0; //reinicio intentos de entrar en el sistema

          
        }

        Serial.println("INGRESE LA CONTRASEA: ");
      }      

      Entrada =""; //LIMPIEZA DE VARIABLE DE CONTRASEÑA 
    } else{
      Entrada += String(Ingreso); //INGRESO DE CONTRASEÑA A CADENA DE TEXTO
      Serial.print("*"); //MOSTRAR DATO INGRESADO DE MANERA INCOGNITA
    }
    

  }


  }
  


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

  pinMode(ledPin1, OUTPUT); //Configura los led de señal advertencia
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  pinMode(switchPin, INPUT);  // Configura el pin del switch como entrada

}

void loop() {
  SistemaDeSeguridad();
}
