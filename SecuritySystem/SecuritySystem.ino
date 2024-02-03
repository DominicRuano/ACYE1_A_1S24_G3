#include <Key.h>
#include <Keypad.h>

const byte Filas = 4;
const byte Columnas = 4;

char Teclado[Filas][Columnas] = {

{'7','8','9','A'},
{'4','5','6','B'},
{'1','2','3','C'},
{'*','0','#','D'}
};

byte Fpines[Filas] = {5,4,3,2};
byte Cpines[Columnas] = {11,10,9,8};

Keypad teclado = Keypad(makeKeymap(Teclado), Fpines, Cpines, Filas, Columnas);

void setup() {
  
  Serial.begin(9600);

}

void loop() {
  

  String Pasword = "AC6C124";
  
  char Ingreso = teclado.getKey();

  if (Ingreso){
     Serial.print(Pasword);
    Serial.print("INGRESE LA CONTRASEA: ");
    Serial.print("tecla ingresada: ");
    Serial.print(Ingreso);
  }
  
}
