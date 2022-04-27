/*
Author : Mohamad Nur Fitri bin Abd Halim
Title : Arduino Receiver LiFi Code

*/
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27,16,2);
void LCD1602_init(void) //液晶初始化用到的函数，包罗在这里
{
lcd.init(); //调用LiquidCrystal_I2C.h里的LCD初始化函数
delay(10); //延时10毫秒
lcd.backlight(); //开LCD1602的背光灯
lcd.clear(); //清除屏幕
}
int x=0;
const int selangmasa=1;
int a[7];

void setup() 
{
  Serial.begin(9600);
  LCD1602_init();
 
  
  lcd.setCursor(0, 0); 
  lcd.print(" Wait Signal "); 
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
}

void loop() 
{
  
  if(digitalRead(0)==LOW)
  {
    for (int i=0;i<7;i++)
    { 
      while(digitalRead(0)!=LOW);
      delay(4*selangmasa);
      a[i]=!digitalRead(0);
      delay(4*selangmasa); 
    }
    x=x+1;
    delay(8*selangmasa);
    int m=decimal(a);
    char l= char(m);
    Serial.print(l);
    lcd.print(l);
    lcd.setCursor(x, 0);
    if(x>15)
    {x=0;
    lcd.clear();}
  }
}

int decimal(int c[])
{
  int i, j=0 , decimal=0, temp;
  for(i=6;i>=0;i--)
  {
    temp=c[i];
    for(int m=0;m<j;m++) 
    temp=temp*2;
    decimal=decimal +temp;
    j++;
  }
  return decimal;
}
