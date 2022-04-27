/*
Author : Mohamad Nur Fitri bin Abd Halim
Title : Arduino Transimitter LIFI code
*/

int incomingByte = 0; // serial data that is receive from serial monitor
const int selangmasa=1; 
const int LED =13;  

void setup() 
{
  pinMode(LED, OUTPUT); 
  Serial.begin(9600); 
}

void loop() 
{
  if (Serial.available() > 0) 
  {
    incomingByte = Serial.read(); 
    int a[10], i ;       
    for(i=0; incomingByte>0; i++)    
    {    
      a[i]=incomingByte%2;    
      incomingByte= incomingByte/2;  
    }    
  
    for(i=i-1 ;i>=0 ;i--)
    {   
      digitalWrite(LED,HIGH);
      delay(2*selangmasa);
      digitalWrite(LED,a[i]);
      delay(4*selangmasa); 
      digitalWrite(LED,LOW);
      delay(4*selangmasa);
    }    
  }  
}
