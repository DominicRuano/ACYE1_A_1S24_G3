#include <Key.h>
#include <Keypad.h>

const byte Filas = 4;
const byte Columnas = 4;

char Teclado[Filas][Columnas] = {

{'1','2','3','+'},
{'4','5','6','-'},
{'7','8','9','x'},
{'*','0','#','/'}
};

byte Fpines[Filas] = {5,4,3,2};
byte Cpines[Columnas] = {11,10,9,8};

Keypad Teclado = Keypad(makeKeymap(Teclado), Fpines, Cpines, Filas, Columnas);

void setup() {
  
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

}
