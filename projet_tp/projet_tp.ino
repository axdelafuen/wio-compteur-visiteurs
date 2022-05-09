#include "TFT_eSPI.h"

TFT_eSPI tft;
unsigned int visiteurs = 0;
unsigned int affluence = 0;

void setup() {
  tft.begin();
  tft.setRotation(3); // Définis le coin de l'écran de coordonnées 0,0
  Serial.begin(115200);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  display(0);
  loop();
}

void display(int test){
  if(test==0){tft.fillScreen(TFT_GREEN);} // fond vert
  if(test==1){tft.fillScreen(TFT_RED);}
  tft.fillRect(0,0,320,50,TFT_BLUE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK,TFT_BLUE);
  tft.drawString("+",10,10);
  tft.drawString("-",80,10);
  tft.drawString("RAZ",150,10);
  if(test==0){tft.setTextColor(TFT_BLACK, TFT_GREEN);}
  if(test==1){tft.setTextColor(TFT_BLACK, TFT_RED);}
  tft.drawString("Visiteurs :",10,70);
  tft.drawString("Affluence :",10,115);
  tft.setCursor(215,70);
  tft.print(visiteurs);
  tft.setCursor(215,115);
  tft.print(affluence);
}

bool dataVerif(){
  if(affluence == 0){return false;}
  return true;
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

void loop() {
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
