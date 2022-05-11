#include "TFT_eSPI.h" 
#include "RTC_SAMD51.h"
#include "DateTime.h"

RTC_SAMD51 rtc;
TFT_eSPI tft;
DateTime now = DateTime(F(__DATE__), F(__TIME__));
DateTime tmp = DateTime(F(__DATE__), F(__TIME__));
unsigned int visiteurs = 0;
unsigned int affluence = 0;
unsigned int isSerial = 2;
char tabChar[100];

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

void display(int displayType){
  if(displayType==0 || displayType==3){tft.fillScreen(TFT_GREEN);} // fond vert
  if(displayType==1 || displayType==4){tft.fillScreen(TFT_RED);}
  tft.fillRect(0,0,320,50,TFT_BLUE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLACK,TFT_BLUE);
  tft.drawString("+",10,10);
  tft.drawString("-",80,10);
  tft.drawString("RAZ",150,10);
  dispTime();
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
  if(now.minute()<10 && now.hour()<10){
    tft.print("0"+String(now.hour(),DEC)+":0"+String(now.minute(),DEC));  
  }
  else if(now.minute()<10){
      tft.print(String(now.hour(),DEC)+":0"+String(now.minute(),DEC));  
  }
  else if(now.hour()<10){
      tft.print("0"+String(now.hour(),DEC)+":"+String(now.minute(),DEC));  
  }
  else {
    tft.print(String(now.hour(),DEC)+":"+String(now.minute(),DEC));
  }
  if(affluence<11){
    tft.setTextColor(TFT_BLACK,TFT_GREEN);
  }
  else{
    tft.setTextColor(TFT_BLACK,TFT_RED);
  }
  tmp=now;
}

void DispTimeSerial(){
  Serial.println("-------------");
  Serial.print("Heure actuelle : ");
  rtc.adjust(now);
  if(now.hour()<10){
    Serial.print("0"+String(now.hour(), DEC));
  }
  else{
    Serial.print(now.hour(), DEC);
  }
  Serial.print(':');
  if(now.minute()<10){
    Serial.print("0"+String(now.minute(), DEC));
  }
  else{
    Serial.println(now.minute(), DEC);
  }
  
}

void newMessage(){
  if(affluence < 11){
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    tft.drawString("Nouveau message",10,160);  
  }
  else{
    tft.setTextColor(TFT_BLACK, TFT_RED);
    tft.drawString("Nouveau message",10,160);
  }
  analogWrite(WIO_BUZZER, 128);
  delay(250);
  analogWrite(WIO_BUZZER, 100);
  delay(250);
  analogWrite(WIO_BUZZER, 50);
  delay(250);
  analogWrite(WIO_BUZZER, 0);
  isSerial = 3;
}

void affichMessage(char msg[]){
  int i = 0;
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString(msg,0,0);
  tft.drawString("Down pour sortir",10,200);

  while(i<3000){
    if (digitalRead(WIO_5S_DOWN) == LOW){
      if(affluence < 11){display(0);}
      else{display(1);}
      isSerial=0;
      return;  
    }
    i+=10;
    delay(100); 
  } 
}

//// STATS ////

void incrementation(bool isRemiseAZero){
  visiteurs = visiteurs+1;
  affluence = affluence+1;
  if(affluence==11){
    if(isRemiseAZero==false){\
      display(1);
      analogWrite(WIO_BUZZER, 10);
      delay(500);
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

bool dataVerif(){
  if(affluence == 0){return false;}
  return true;
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
      Serial.println("-------------");
      Serial.println("Remise des compteurs à zéro !");
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

void envoyerLesStats(){
  Serial.println();
  DispTimeSerial();
  Serial.print("Visiteurs totales : ");
  Serial.println(visiteurs,DEC);
  Serial.print("Affluence actuelle : ");
  Serial.println(affluence,DEC);
  Serial.println("-------------");
}

//// SETUP ////

void setup() {
  tft.begin(); // lancer tft
  rtc.begin(); // lancer rtc
  Serial.begin(115200); // communiaction avec le moniteur de série en 115200 baud 
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // on ajuste rtc sur l'heure de compilation pour que le Wio soit autonome sur l'heure
  needSerialCom(); // lance la fonction qui vérifie que le moniteur série est lancé
  DispTimeSerial(); // affiche l'heure de début sur le moniteur de série

  pinMode(WIO_KEY_A, INPUT_PULLUP); 
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  display(0); // affichage de base
}

//// LOOP ////

void loop() {

  int carDispo = Serial.available();
  if(carDispo > 0){
    int i = 0;
    while(carDispo > 0)
    {
        char carlu = Serial.read();
        tabChar[i] = carlu;
        carDispo = Serial.available();
        i = i + 1;
        isSerial = 1;
    }
  }
  
  if (isSerial == 1){
    newMessage();  
  }

  if (isSerial != 2){
    if (digitalRead(WIO_5S_UP) == LOW){
      affichMessage(tabChar);  
    }  
  }

  if (isSerial == 0){
    if (affluence < 11){
      tft.fillRect(10,160,270,30,TFT_GREEN);  
    }
    else{
      tft.fillRect(10,160,150,10,TFT_RED);  
    }
  }
 
  now=rtc.now(); // mise a jour de la variable now 
  
  if(tmp.minute()!=now.minute()||tmp.hour()!=now.hour()){ // vérif si le dernier horaire est dépasser
    dispTime();
    if((now.minute()%15)==0){ // si le temps a changer on regarde si les minutes ne sont pas multiple de 15 pour envoyer les infos
      envoyerLesStats();
    }
  }

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
