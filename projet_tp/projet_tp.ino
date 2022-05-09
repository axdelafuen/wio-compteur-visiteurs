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
  pinMode(WIO_BUZZER, OUTPUT);
  display(0);
  loop();
}

void display(int displayType){
  if(displayType==0 || displayType==3){tft.fillScreen(TFT_GREEN);} // fond vert
  if(displayType==1 || displayType==4){tft.fillScreen(TFT_RED);}
  tft.fillRect(0,0,320,50,TFT_BLUE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK,TFT_BLUE);
  tft.drawString("+",10,10);
  tft.drawString("-",80,10);
  tft.drawString("RAZ",150,10);
  if(displayType==0 || displayType==3){tft.setTextColor(TFT_BLACK, TFT_GREEN);}
  if(displayType==1 || displayType==4){tft.setTextColor(TFT_BLACK, TFT_RED);}
  tft.drawString("Visiteurs :",10,70);
  tft.drawString("Affluence :",10,115);
  tft.setCursor(215,70);
  tft.print(visiteurs);
  tft.setCursor(215,115);
  tft.print(affluence);
  if(displayType==3 || displayType==4){tft.drawString("Confirmez ?",110,200);}
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

void incrementation(bool isRemiseAZero){
  visiteurs = visiteurs+1;
  affluence = affluence+1;
  if(affluence==11){
    if(isRemiseAZero==false){\
      display(1);
      analogWrite(WIO_BUZZER, 128);
      delay(1000);
      analogWrite(WIO_BUZZER, 0);
    }
    else{
      display(4);  
      analogWrite(WIO_BUZZER, 128);
      delay(1000);
      analogWrite(WIO_BUZZER, 0);
    }
  }
  affichResults();
  delay(200);    
}

void decrementation(bool isRemiseAZero){
  if(dataVerif()){
      affluence = affluence-1; 
    }
    if(affluence==10){
      if(isRemiseAZero==false){
        display(0);
      }
      else{
        display(3);  
      }  
    }
    affichResults();
    delay(200); 
}

void RemiseAZero(){
  int i = 0;
  tft.drawString("Confirmez ?",110,200);
  while(i < 500){
    if (digitalRead(WIO_KEY_C) == LOW){
      incrementation(true);
      i+=20;
      continue;
    }
    if (digitalRead(WIO_KEY_B) == LOW){
      decrementation(true);
      i+=20;
      continue;
    }
    if(digitalRead(WIO_5S_PRESS) == LOW){
      visiteurs=0;
      affluence=0;
      display(0);
      return;
    }
    i+=10;
    delay(100);
  }
  if(affluence  < 11){
    tft.fillRect(110,200,200,50,TFT_GREEN);
  }
  else{
     tft.fillRect(110,200,200,50,TFT_RED);
  }
}

void loop() {
  if (digitalRead(WIO_KEY_C) == LOW){
    incrementation(false);
  }
  if (digitalRead(WIO_KEY_B) == LOW){
    decrementation(false);
  }
  if (digitalRead(WIO_KEY_A) == LOW){
   RemiseAZero();
  }
} 
