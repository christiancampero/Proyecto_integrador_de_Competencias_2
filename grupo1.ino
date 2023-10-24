#include <SoftwareSerial.h>
SoftwareSerial BT(10,11);
int comm[40];
int p[6];
int s[6];
int msg[9];
int i = 0;

void setup(){
  Serial.begin(9600);
  BT.begin(38400);

  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
}
 
void loop(){
  i = 0;
  if(BT.available()){
    while(BT.available()){
      comm[i] = BT.read();
      i++;
    }

    switch(comm[3]){
    case 80:
      digitalWrite(7, HIGH); // Prueba de led

      for(int k = 0; k < 6; k++){
        p[k] = (comm[k+4] - 48);  // Conversión ascii a decimal
      }

      Serial.write("P");
      break;
    case 83:
      digitalWrite(6, HIGH); // Prueba de led

      for(int k = 0; k < 6; k++){
        s[k] = (comm[k+4] - 48);  // Conversión ascii a decimal
      }

      Serial.write("S");
      break;
    case 86: // Averiguar como enviar respuesta
      digitalWrite(5, HIGH); // Prueba de led
      Serial.write("V");
      break;
    default:
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      break;
    }
  }

  Serial.println("==");
  for(int j = 0; j < 6; j++){
    Serial.println(s[j]);   // Ver que se guarde correctamente
  }
  Serial.println("==");
  
}