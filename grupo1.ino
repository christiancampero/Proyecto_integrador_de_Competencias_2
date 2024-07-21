#include <SoftwareSerial.h>

#define BUT1 A0
#define BUT2 A1
#define BUT3 A2
#define BUT4 A3
#define BUT5 A4
#define BUT6 A5
#define BOFF 6

#define LED_RED 5
#define LED_GREEN 3

#define LATCH 4
#define RELOJ 2
#define DATOS 7

SoftwareSerial BT(10, 11);

unsigned int p[6] = {0,0,0,0,0,0};
unsigned int stock[6] = {0, 0, 0, 0, 0, 0};
unsigned int comm[20];
byte m, n;
unsigned int cont = 0;
unsigned char numero[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90,0XFF};
bool e = false;

void display();
void displayOFF();
void ledOFF();
void BUT_OFF(int Bu1, int Bu2, int Bu3, int Bu4, int Bu5, int Bu6);
void comm_reset();

void setup() { 
  Serial.begin(9600); // Baud de estandar.
  BT.begin(9600); // Baud de estandar.
  
  pinMode(BUT1,INPUT);
  pinMode(BUT2,INPUT);
  pinMode(BUT3,INPUT);
  pinMode(BUT4,INPUT);
  pinMode(BUT5,INPUT);
  pinMode(BUT6,INPUT);
  
  pinMode(BOFF, INPUT);
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  pinMode(LATCH, OUTPUT);
  pinMode(RELOJ, OUTPUT);
  pinMode(DATOS, OUTPUT);

  Serial.println("Listo para conectarse.");
}

void loop() {

  e = false;
  cont = 0;
  
  if(BT.available()){
    while(BT.available()){
      comm[cont] = BT.read();
      cont++;
      delay(20);
    }

    switch(comm[3]){
      case 66: // Ascii para "B", realizar pedido.
        for(int k = 0; k < 6; k++){
          p[k] = (comm[k+4] - 48);

          if(p[k] > stock[k]){  // Evalúa si hay stock.
            e = true;
            break;
          }
        }

        if(e){
          BT.write("AT$E\r\n"); // Mensaje de error de stock.
        } else{
          for(int k = 0; k < 6; k++){
            stock[k] = stock[k] - p[k];
          }
          BT.write("OK\r\n"); // Respuesta de no error.
          digitalWrite(LED_RED, HIGH);  // Indicador de realizando pedido.
          m = 0;
        }
      
        comm_reset();
        break;

      case 67:  // Ascii para "C", mostrar stock.
        BT.write("AT$");  // Mensaje de respuesta a mostrar stock.
        for(int k = 0; k < 5; k++){
          BT.write((stock[k] + 48));
          BT.write(",");
        }
        BT.write((stock[5] + 48));
        BT.write("\r\n");
        
        digitalWrite(LED_GREEN, HIGH);  // Indicador de mostrando stock.
        m = 1;
        
        comm_reset();
        break;

      case 65:  // Ascii para "A", actualizar stock.
        int kt = 0;
        for(int k = 0; k < 6; k ++){
          stock[k] += (comm[kt+4] - 48);
          kt += 2;
        }
        
        BT.write("AT$");  // Mensaje de respuesta a actualizar stock.
        for(int k = 0; k < 5; k++){
          BT.write((stock[k] + 48));
          BT.write("*");
        }
        BT.write((stock[5] + 48));
        BT.write("\r\n");
        
        comm_reset();
        break;

      case '\n':
        break;

      default:
        comm_reset();
        break;
    }
  }

  if(digitalRead(BOFF) == 1){
   
    displayOFF();

  } else if(digitalRead(BOFF) == 0 && m == 0){
   
    display();

  } else if(digitalRead(BOFF) == 0 && m == 1){
   
    displayStock();

  }

  BUT_OFF(analogRead(BUT1), analogRead(BUT2), analogRead(BUT3), analogRead(BUT4), analogRead(BUT5), analogRead(BUT6));

  ledOFF();

}

void display(){
  for(byte i = 0; i < 6; i++){
   if(p[i] == 0){
    p[i] = 10;
   }
  }

  for(byte i = 0; i < 6; i++){
    digitalWrite(LATCH, LOW);
    shiftOut(DATOS, RELOJ, MSBFIRST, numero[p[i]]);
    shiftOut(DATOS, RELOJ, MSBFIRST, B00000001 << i); // ver si es en ese orden o al reves
    digitalWrite(LATCH, HIGH);
    delay(2);
  }
}

void displayStock(){
  for(byte i = 0; i < 6; i++){
    digitalWrite(LATCH, LOW);
    shiftOut(DATOS, RELOJ, MSBFIRST, numero[stock[i]]);
    shiftOut(DATOS, RELOJ, MSBFIRST, B00000001 << i);  // ver si es en ese orden o al reves
    digitalWrite(LATCH, HIGH);
   	delay(2);
  }
}

void displayOFF(){
  for(byte i = 0; i < 6; i++){
    p[i] = 10;
  }
  
  for(byte i = 0; i < 6; i++){
    digitalWrite(LATCH, LOW);
    shiftOut(DATOS, RELOJ, MSBFIRST, numero[10]);
    shiftOut(DATOS, RELOJ, MSBFIRST, B00000001 << i);
    digitalWrite(LATCH, HIGH);
   	delay(2);
  }
  
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);

  m = -1;
  
}

void ledOFF(){
  
  byte ver = 0;
  
  for(byte i = 0; i < 6; i++){
    if(p[i] == 10){
    ver++;
    }
  }
  
  if(ver == 6){
    digitalWrite(LED_RED, LOW);
  }
}

void BUT_OFF(int Bu1, int Bu2, int Bu3, int Bu4, int Bu5, int Bu6){
  
  if(Bu1 >= 990){

    p[0] = 10;
    
  } else if(Bu2 >= 990){
    
    p[1] = 10;
    
  } else if(Bu3 >= 990){
    
    p[2] = 10;
    
  } else if(Bu4 >= 990){
    
    p[3] = 10;
    
  } else if(Bu5 >= 990){
    
    p[4] = 10;
    
  } else if(Bu6 >= 990){
    
    p[5] = 10;
    
  }
}

void comm_reset(){
  for(byte i = 0; i < 20; i++){
    comm[i] = 0;
  }
}