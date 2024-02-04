#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Key.h>
#include <Keypad.h>

/*Keyboard */

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'7','8','9','/'},
  {'4','5','6','*'},
  {'1','2','3','-'},
  {'C','0','=','+'}
};

byte pinsRows[ROWS] = {9,8,7,6};
byte pinsCols[COLS] = {5,4,3,2};

Keypad keypad = Keypad(makeKeymap(keys), pinsRows, pinsCols, ROWS, COLS);

/*Animacion -> Resultado*/

#define USE_UI_CONTROL 0

#if USE_UI_CONTROL
#include <MD_UISwitch.h>
#endif

#define DEBUG 0

#if DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#if USE_UI_CONTROL
const uint8_t SPEED_IN = A5;
const uint8_t DIRECTION_SET = 8;
const uint8_t INVERT_SET = 9; 

const uint8_t SPEED_DEADBAND = 5;
#endif

uint8_t scrollSpeed = 100;    
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 0;

#define	BUF_SIZE	75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "--------" };
bool newMessageAvailable = true;

#if USE_UI_CONTROL

MD_UISwitch_Digital uiDirection(DIRECTION_SET);
MD_UISwitch_Digital uiInvert(INVERT_SET);

void doUI(void)
{
  {
    int16_t speed = map(analogRead(SPEED_IN), 0, 1023, 10, 150);

    if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) ||
      (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
    {
      P.setSpeed(speed);
      scrollSpeed = speed;
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }

  if (uiDirection.read() == MD_UISwitch::KEY_PRESS) 
  {
    PRINTS("\nChanging scroll direction");
    scrollEffect = (scrollEffect == PA_SCROLL_LEFT ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT);
    P.setTextEffect(scrollEffect, scrollEffect);
    P.displayClear();
    P.displayReset();
  }
  
  if (uiInvert.read() == MD_UISwitch::KEY_PRESS)  
  {
    PRINTS("\nChanging invert mode");
    P.setInvert(!P.getInvert());
  }
}
#endif 

void readSerial(void)
{
  static char *cp = newMessage;

  while (Serial.available())
  {
    *cp = (char)Serial.read();
    if ((*cp == '\n') || (cp - newMessage >= BUF_SIZE-2)) 
    {
      *cp = '\0';
      cp = newMessage;
      newMessageAvailable = true;
    }
    else 
      cp++;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline");

#if USE_UI_CONTROL
  uiDirection.begin();
  uiInvert.begin();
  pinMode(SPEED_IN, INPUT);

  doUI();
#endif 

  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);

  Serial.println("Teclado 4*4 con Blblioteca Keypad: ");
  Serial.println();
}

/*Stack para manejar operaciones multiples*/
class Stack {
  public:
    Stack(int size) {
      _size = size;
      _data = new int[_size];
      _top = -1;
    }
    ~Stack() {
      delete[] _data;
    }
    void push(int value) {
      if (_top < _size - 1) {
        _data[++_top] = value;
      }
    }
    int pop() {
      if (_top >= 0) {
        return _data[_top--];
      }
      return 0;
    }
    int top() {
      if (_top >= 0) {
        return _data[_top];
      }
      return 0;
    }
    bool empty() {
      return _top < 0;
    }
  private:
    int _size;
    int* _data;
    int _top;
};

// Almacenamos la expresion a evaluar
String expr = ""; 

// Iniciamos el contador en 0 para el tiempo de espera para las operaciones
unsigned long resultDisplayTime = 0;

bool error = false; 

int applyOp(int a, int b, char op) {
  switch(op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': 
      if(b == 0){
        Serial.println("Error: Division por cero");
        error = true; 
        strcpy(newMessage, "ERROR: DIVISION POR CERO"); 
        newMessageAvailable = true; 
        return 0; 
      } else {
        return a / b;
      }
  }
}

void readKeypad(void){
  if (millis() - resultDisplayTime < 30000) {
    return;
  }

  char key = keypad.getKey();
  if (key){
    Serial.print(key);
    switch(key) {
      case '0'...'9':
        expr += key;
        break;
      case '+':
      case '-':
      case '*':
      case '/':
        expr += String(" ") + key + String(" ");
        break;
      case '=':
        error = false;
        int result = evaluateExpression(expr);
        expr = "";
        if (!error) {
          String resultString = "RESULTADO = " + String(result);
          resultString.toCharArray(newMessage, BUF_SIZE);
        }
        newMessageAvailable = true;
        resultDisplayTime = millis();
        break;
      case 'C':
        expr = "";
        P.displayClear();
        newMessage[0] = '\0'; 
        newMessageAvailable = false; 
        break;
    }
  }
}


int evaluateExpression(String expr) {
  Stack values(50);
  Stack ops(50); 
  
  for (int i = 0; i < expr.length(); i++) {
    if (expr[i] == ' ')
      continue;
    else if (isdigit(expr[i])) {
      int val = 0;
      while (i < expr.length() && isdigit(expr[i])) {
        val = (val*10) + (expr[i]-'0');
        i++;
      }
      values.push(val);
      i--;
    } else {
      while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
        int val2 = values.top();
        values.pop();

        int val1 = values.top();
        values.pop();

        char op = ops.top();
        ops.pop();

        values.push(applyOp(val1, val2, op));
      }
      ops.push(expr[i]);
    }
  }

  while (!ops.empty()) {
    int val2 = values.top();
    values.pop();

    int val1 = values.top();
    values.pop();

    char op = ops.top();
    ops.pop();

    values.push(applyOp(val1, val2, op));
  }

  return values.top();
}

int precedence(char op) {
  if (op == '+' || op == '-')
    return 1;
  if (op == '*' || op == '/')
    return 2;
  return 0;
}

void loop()
{
  #if USE_UI_CONTROL
    doUI();
  #endif 
  if (P.displayAnimate())
  {
    P.displayReset();
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
      P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
    }
  }
  readSerial();
  readKeypad();
}
