#include "TFT_eSPI.h" 
#include "RTC_SAMD51.h"
#include "DateTime.h"

RTC_SAMD51 rtc;
TFT_eSPI tft;
DateTime now = DateTime(F(__DATE__), F(__TIME__));
unsigned int visiteurs = 0;
unsigned int affluence = 0;

//// NEED SERIAL COM ////

void needSerialCom(){
  tft.fillScreen(TFT_GREEN);
  tft.setRotation(3);
  tft.setTextSize (3);
  tft.setTextColor (TFT_BLACK, TFT_GREEN);
  tft.drawString("Need serial com",4,10);
  while (!Serial)
  {
    ;
  }
}

//// AFFICHAGE ////

void display(int test){
  if(test==0){tft.fillScreen(TFT_GREEN);} // fond vert
  if(test==1){tft.fillScreen(TFT_RED);}
  tft.fillRect(0,0,320,50,TFT_BLUE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK,TFT_BLUE);
  tft.drawString("+",10,10);
  tft.drawString("-",80,10);
  tft.drawString("RAZ",150,10);
  dispTime();
  if(test==0){tft.setTextColor(TFT_BLACK, TFT_GREEN);}
  if(test==1){tft.setTextColor(TFT_BLACK, TFT_RED);}
  tft.drawString("Visiteurs :",10,70);
  tft.drawString("Affluence :",10,115);
  tft.setCursor(215,70);
  tft.print(visiteurs);
  tft.setCursor(215,115);
  tft.print(affluence);
}

void affichResults(){
  tft.setCursor(215,70);
  tft.print(visiteurs);
  tft.setCursor(215,115);
  if(affluence==9){
    tft.fillRect(233,110,20,30,TFT_GREEN);  
  }
  tft.print(affluence);
}


void dispTime(){
  tft.setTextColor(TFT_BLACK, TFT_BLUE);
  tft.setCursor(220,10);
  if(now.minute()<10){
    tft.print(String(now.hour(),DEC)+":0"+String(now.minute(),DEC));  
  }
  else {
    tft.print(String(now.hour(),DEC)+":"+String(now.minute(),DEC));
  }
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
}

void DispTimeSerial(){
  Serial.println("Heure actuel : ");
  rtc.adjust(now);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.println();
  Serial.println("-------------\n");
}

//// STATS ////

void incrementation(){
  visiteurs = visiteurs+1;
  affluence = affluence+1;
  if(affluence==11){
    display(1);  
  }
  affichResults();
  delay(200);    
}

void decrementation(){
  if(dataVerif()){
      affluence = affluence-1; 
    }
    if(affluence==10){
      display(0);  
    }
    affichResults();
    delay(200); 
}

bool dataVerif(){
  if(affluence == 0){return false;}
  return true;
}

void RemiseAZero(){
  int i = 0;
  tft.drawString("Confirmez ?",110,200);
  while(i < 500){
    if (digitalRead(WIO_KEY_C) == LOW){
      incrementation();
    }
    if (digitalRead(WIO_KEY_B) == LOW){
      decrementation();
    }
    if(digitalRead(WIO_5S_PRESS) == LOW){
      visiteurs=0;
      affluence=0;
      display(0);
      break;
    }
    i+=10;
    delay(10);
  }
}

void envoyerLesStats(){
  Serial.println('Stats');
}

//// SETUP ///

void setup() {
  tft.begin();
  rtc.begin();
  Serial.begin(115200);

  needSerialCom();

  now = rtc.now();
  DispTimeSerial();

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  display(0);
}

//// LOOP ////

void loop() {
  if(now!=rtc.now()){
    now = rtc.now();
    dispTime();
  }
  if((now.minute()%15)==0){
    envoyerLesStats();
  }

  if (digitalRead(WIO_KEY_C) == LOW){
    incrementation();
  }
  if (digitalRead(WIO_KEY_B) == LOW){
    decrementation();
  }
  if (digitalRead(WIO_KEY_A) == LOW){
   RemiseAZero();
  }
  
}
