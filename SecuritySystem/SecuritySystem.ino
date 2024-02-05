#include <Key.h>
#include <Keypad.h>

const byte Filas = 4; //tamaño de filas keypad
const byte Columnas = 4; //tamaño de columnas keypad

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
  
  Serial.begin(9600);

}

void loop() {
  
  
  char Ingreso = teclado.getKey(); // VARIABLE QUE CAPTURA LA ENTRADA DE CARACTERES POR MEDIO DEL KEYPAD

  if (Entradas == 0){ //MENSAJE DE BIENVENIDA AL USUARIO
    Serial.println("||----BIENVENIDO USUARIO DEL GRUPO 6----||");
    Serial.print("INGRESE LA CONTRASEA: ");
    Entradas +=1;
  }

  if (Ingreso){ //COMPROBACION DE QUE NO SE HA INGRESADO NADA DENTRO DE LA VARIABLE

    if (Ingreso == '#'){ //UTILIZANDO EL SIGNO * COMO ENTER PARA PODER INGRESAR CONTRASEÑA
      Serial.println();
      Serial.print("CONTRASEÑA INGRESADA: ");
      Serial.println(Entrada);
      
      if (Entrada == Pasword){
        Serial.println("||---- BIENVENIDO!----||"); //SI LA CONTRASEÑA ES CORRECTA DA MENSAJE DE BIENVENIDA
        
        while (salida){

          char key = teclado.getKey();
          if (key == '*') { // cambio de contraseña
            Serial.print("INGRESE NUEVA CONTRASEÑA: ");

            Pasword = ""; // limpiar la nueva contraseña

            while (true) {
              key = teclado.getKey();

              if (key == '#') {
                if (Pasword.length() >= 6 && Pasword.length() <= 8) {
                  Serial.println();
                  Serial.println("Nueva Contraseña: " + Pasword);
                  break;
                } else {
                  Serial.println();
                  Serial.println("TAMAÑO DE LA CONTRASEÑA INCORRECTO!");
                  Serial.println("Longitud actual: " + String(Pasword.length()));
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
        Serial.print("INGRESE LA CONTRASEA: ");
      }
        

      Entrada =""; //LIMPIEZA DE VARIABLE DE CONTRASEÑA 
    } else{
      Entrada += String(Ingreso); //INGRESO DE CONTRASEÑA A CADENA DE TEXTO
      Serial.print("*"); //MOSTRAR DATO INGRESADO DE MANERA INCOGNITA
    }
    
    



  }
  
}
