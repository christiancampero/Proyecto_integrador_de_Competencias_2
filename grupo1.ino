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

SoftwareSerial BT(10,11);

unsigned int p[6] = {0,0,0,0,0,0};
unsigned int stock[6] = {0, 0, 0, 0, 0, 0};
unsigned int comm[20];
byte m, n;
unsigned int cont = 0;
unsigned char numero[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90,0XFF};

void display();
void displayOFF();
void ledOFF();
void BUT_OFF(int Bu1, int Bu2, int Bu3, int Bu4, int Bu5, int Bu6);
void comms_init();

void setup() {
  
  Serial.begin(9600);
  BT.begin(38400);
  
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

  comms_init();

}

void loop() {

  cont = 0;
  
  if(BT.available()){

    while(BT.available()){
      comm[cont] = BT.read();
      Serial.print(comm[cont]); // POR QUE NO FUNCIONA SIN ESTA LINEA?
      cont++;
    }

    switch(comm[3]){

    case 80:
      for(int k = 0; k < 6; k++){
        p[k] = (comm[k+4] - 48);

        stock[k] = stock[k] - p[k];
      }
    
      Serial.println("Pedido ingresado correctamente.");
      digitalWrite(LED_RED, HIGH);
      m = 0;
      break;

    case 83:
      for(int k = 0; k < 6; k++){
        stock[k] = (comm[k+4] - 48);
      }

      Serial.println("Stock actualizado correctamente.");
      break;

    case 86:
      Serial.println("El stock actual es:");
      Serial.print("Diodo [101]: ");
      Serial.println(stock[0]);
      Serial.print("Transistor [102]: ");
      Serial.println(stock[1]);
      Serial.print("Capacitor [103]: ");
      Serial.println(stock[2]);
      Serial.print("LED [201]: ");
      Serial.println(stock[3]);
      Serial.print("Resistencia 1K [202]: ");
      Serial.println(stock[4]);
      Serial.print("Resistencia 330 [203]: ");
      Serial.println(stock[5]);
      
      digitalWrite(LED_GREEN, HIGH);
      m = 1;
      break;

    default:
      Serial.println("Comando incorrecto.");
      Serial.println("Los comandos disponibles son:");
      Serial.println("[P] para realizar un pedido.");
      Serial.println("[S] para actualizar el Stock.");
      Serial.println("[V] para visualizar el Stock actual.");

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

  BUT_OFF(digitalRead(BUT1), digitalRead(BUT2), digitalRead(BUT3), digitalRead(BUT4), digitalRead(BUT5), digitalRead(BUT6));

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
  
  if(Bu1 == 1){
    
    p[0] = 10;
    
  } else if(Bu2 == 1){
    
    p[1] = 10;
    
  } else if(Bu3 == 1){
    
    p[2] = 10;
    
  } else if(Bu4 == 1){
    
    p[3] = 10;
    
  } else if(Bu5 == 1){
    
    p[4] = 10;
    
  } else if(Bu6 == 1){
    
    p[5] = 10;
    
  }
}

void comms_init(){
  Serial.println("Iniciando...");
  delay(200);

  BT.print("AT\r\n");
  delay(200);

  BT.print("AT+ROLE?\r\n");
  delay(200);  

  BT.print("AT+UART?\r\n");
  delay(200);

  BT.print("AT+ADDR?\r\n");
  delay(200);

  while(BT.available())
    Serial.write(BT.read());

  Serial.println("Listo para conectarse.");
}