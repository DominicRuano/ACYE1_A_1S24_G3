#include <Key.h>
#include <Keypad.h>

const byte Filas = 4; //tamaño de filas keypad
const byte Columnas = 4; //tamaño de columnas keypad

//LED DE ADVERTENCIA ERRORES EN CONTRASEÑA
const int ledPin1 = 7;
const int ledPin2 = 12;
const int ledPin3 = 13;

//SWITCH PARA VERIFICAR EL ESTADO DE CALCULADORA 
const int switchPin = 6;  // Define el pin al que está conectado el switch
int switchState = 0;      // Variable para almacenar el estado del switch
int lastSwitchState = 0;  // Variable para almacenar el estado anterior del switch

char Teclado[Filas][Columnas] = {

{'7','8','9','A'},
{'4','5','6','B'},
{'1','2','3','C'},
{'*','0','#','D'}
}; //Disposicion de las teclas del keypad

byte Fpines[Filas] = {5,4,3,2}; // pines de entrada del keypad para las filas
byte Cpines[Columnas] = {11,10,9,8}; // pines de entrada del keypad para las columnas

Keypad teclado = Keypad(makeKeymap(Teclado), Fpines, Cpines, Filas, Columnas); //asignacion del keupad 

String Entrada; //variable para capturar la contraseña ingresada

int Entradas = 0; //cantidad de veces entrando al sistema

int IntentosEntrada = 0; //Intentos entrar en el sistema fallidos

bool salida = true;

String Pasword = "AC6C124"; //CONTRASEÑA POR DEFECTO DEL SISTEMA


void setup() {

  pinMode(ledPin1, OUTPUT); //Configura los led de señal advertencia
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  pinMode(switchPin, INPUT);  // Configura el pin del switch como entrada

  Serial.begin(9600);

}

void loop() {
  
  
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
        Serial.println("||---- BIENVENIDO!----||"); //SI LA CONTRASEÑA ES CORRECTA DA MENSAJE DE BIENVENIDA
        
        while (salida){

          char key = teclado.getKey();

          switchState = digitalRead(switchPin);  // Lee el estado actual del switch

            // Verifica si el estado del switch ha cambiado
            if (switchState != lastSwitchState) {
              // Si el switch ha sido presionado, Cambia el estado a calculadora
              if (switchState == HIGH) {

                //CONDIGO DE LA CALCULADORA HERE!!!!!!

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

              if (key == '#') { //ACEPTAR NUEVA CONTRASEÑA
                if (Pasword.length() >= 6 && Pasword.length() <= 8) { //COMPROBACION TAMAÑO CORRECTO DE LA NUEVA CONTRASEÑA
                  Serial.println();
                  Serial.println("Nueva Contraseña: " + Pasword); //MOSTRAR CAMBIO DE CONTRASEÑA
                  break; //SALIR DEL BUCLE DE CREADO CONTRASEÑA
                } else {
                  Serial.println();
                  Serial.println("TAMAÑO DE LA CONTRASEÑA INCORRECTO!"); //AVISO DE CONTRASEÑA INCORRECTA
                  Serial.println("Longitud actual: " + String(Pasword.length())); //MOSTRAR TAMAÑO CONTRASEÑA
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
