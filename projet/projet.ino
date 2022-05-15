// ajouts des bibliothèques nécessaires
#include "TFT_eSPI.h" // bibliothèque permettant l'affichage sur le wio terminal
#include "RTC_SAMD51.h" // bibliothèque permettant de récupérer l'heure de compilation du code en se basant sur l'heure de l'ordinateur
#include "DateTime.h" // bibliothèque permattant d'avoir des variables 'chronomètres', ainsi que de les découper en heures / minutes / secondes

//////////////////////////////////////////
//// Declaration des variables global ////
//////////////////////////////////////////

RTC_SAMD51 rtc; 
TFT_eSPI tft; // Declaration du type tft qui permettra d'appeler les fonctions liees a la librairie
DateTime now = DateTime(F(__DATE__), F(__TIME__));
DateTime tmp = DateTime(F(__DATE__), F(__TIME__));
unsigned int visiteurs = 0; // Declaration de de l'attribut visiteurs qui va permettre de connaitre le nombre de visiteurs dans le magasin
unsigned int affluence = 0; // Declaration de l'attribut affluence qui va permettre de connaitre le nombre de visiteus dans le magasin en temps reel
unsigned int isSerial = 2; // Declaration de l'attribut isSerial qui va permettre la lecture et l'affiche du message qu'on envoie depuis le port serie
// On initialise isSerial a 2 au debut. Cela signifie qu'il est impossible d'ouvir l'ecran des messages pour le moment 
char* tabChar; // Declaration de l'attribut tabChar qui est un tableau de caracteres qui nous permettra de stocker le message envoyer sur le port serie

/////////////////////////
//// NEED SERIAL COM ////
/////////////////////////

/**
  *\brief affiche l'écran d'attente du moniteur de série. Cette fonction se termine seulement si le moniteur de s"rie est ouvert.
  */
void needSerialCom(){
  tft.fillScreen(TFT_GREEN); // met le fond en vert
  tft.setRotation(3); // fix le repère graphique
  tft.setTextSize (3); // change la taille du texte en police 3
  tft.setTextColor (TFT_BLACK, TFT_GREEN); // change la couleur du texte et de son arrière-plan
  tft.drawString("Need serial com",4,10); // écrit le texte
  while (!Serial) // attend qu'un moniteur de série soit ouvert.
  {
    ;
  }
}

///////////////////
//// AFFICHAGE ////
///////////////////

/** 
  *\brief Fonction qui permet l'affichage de l'ecran de base avec les fonds vert et bleu ainsi que les informartion comme l'effet du bouton le nombre de visiteurs, l'affluence et l'heure
  *\ elle prend en parametre un int qui va definir certain changement comme la couleur du fond
  */
void display(int displayType){
  if(displayType==0 || displayType==3){tft.fillScreen(TFT_GREEN);} // fond vert si displayType est egal a 0 ou 3
  if(displayType==1 || displayType==4){tft.fillScreen(TFT_RED);} // fond rouge si displayType est egal a 1 ou 4
  tft.fillRect(0,0,320,50,TFT_BLUE); // ajoute le rectangle bleu en haut de l'ecran
  tft.setTextSize(3); // place la taille de la police a 3
  tft.setTextColor(TFT_BLACK,TFT_BLUE); // place la couleur du text en noir et la couleur de fond du texte en bleu pour ecrire sur le rectangle bleu
  tft.drawString("+",10,10);    // 
  tft.drawString("-",80,10);    // qffiche les informations des fonctionnalites des differents boutons sous ceux ci 
  tft.drawString("RAZ",150,10); //
  dispTime(); // appelle la fonction qui permet l'affichage de l'heure
  if(displayType==0 || displayType==3){tft.setTextColor(TFT_BLACK, TFT_GREEN);} // place la couleur du texte en noir et la couleur de fond de celui ci en vert pour ecrire sur le fond vert
  if(displayType==1 || displayType==4){tft.setTextColor(TFT_BLACK, TFT_RED);} // place la couleur du texte en noir et la couleur de fond de celui ci en rouge pour ecrire sur le fond rouge 
  tft.drawString("Visiteurs :",10,70);  // affiche les informations visiteurs et affluence sur l'ecran 
  tft.drawString("Affluence :",10,115);  
  tft.setCursor(215,70); // place un curseur a la suite de Visiteurs et y affiche la valeur de visiteur
  tft.print(visiteurs);  
  tft.setCursor(215,115); // place un curseur a la suite de Affluence et y affiche la valeur de affluence
  tft.print(affluence);
  if(displayType==3 || displayType==4){tft.drawString("Confirmez ?",110,200);} // Si displayType est egale a 3 ou 4, alors affiche en bas de l'ecran le texte "Confirmez ?"
}

/**
  *\brief Fonction qui met a jour les valeurs de Visireurs et Affluence a l'ecran
  */
void affichResults(){
  tft.setCursor(215,70); // place le curseur et ecrit la nouvelle valeur de Visiteurs 
  tft.print(visiteurs);
  tft.setCursor(215,115); // place le curseur et ecrit la nouvelle valeur de Affluence
  if(affluence==9){
    tft.fillRect(233,110,20,30,TFT_GREEN); // Si l'affluence passe en dessous de 10 alors on rajoute un rectangle vert afin d'enlever le 0 qui reste du 10 
  }
  tft.print(affluence);
}

/**
  *\brief fonction qui permet l'affichage de l'heure sur le Wio Terminal
  */
void dispTime(){
  tft.setTextColor(TFT_BLACK, TFT_BLUE); // change la couleur du texte et de son arrière plan 
  tft.setCursor(220,10); // on ce positionne pour écrire au bon endroit
  // Mise en place de conditions pour que l'affichage soit le bon 
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
  // Mise en place de condition pour savoir de quelle couleur de fond du texte doit être remis
  if(affluence<11){
    tft.setTextColor(TFT_BLACK,TFT_GREEN); 
  }
  else{
    tft.setTextColor(TFT_BLACK,TFT_RED);
  }
  tmp=now;
}

/**
  *\brief fonction qui permet d'afficher l'heure actuelle sur le moniteur de serie
  */
void DispTimeSerial(){
  Serial.println("-------------"); 
  Serial.print("Heure actuelle : ");
  rtc.adjust(now); // ajuste l'heure dans la variable rtc
  // Mise en place de conditions pour que l'affichage soit le bon 
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

/**
  *\brief affiche sur l'ecran que l'on a recu un nouveau message si on en a recu un
  */
void newMessage(){
  if(affluence < 11){ 
    // si l'affluence est en dessous de 11 alors on passe la couleur de fond du texte en vert pour s'accorder a la couleur du fond de l'ecran et on ecrit "Nouveau message en bas de l'ecran"
    tft.setTextColor(TFT_BLACK, TFT_GREEN);     
    tft.drawString("Nouveau message",10,160);  
  }
  else{
    // si l'affluence est en dessous de 11 alors on passe la couleur de fond du texte en rouge pour s'accorder a la couleur du fond de l'ecran et on ecrit "Nouveau message en bas de l'ecran"
    tft.setTextColor(TFT_BLACK, TFT_RED);     
    tft.drawString("Nouveau message",10,160);
  } 
  
  // On joue une petie melodie quand on recoi le message

  analogWrite(WIO_BUZZER, 128);
  delay(250);
  analogWrite(WIO_BUZZER, 100);
  delay(250);
  analogWrite(WIO_BUZZER, 50);
  delay(250);
  analogWrite(WIO_BUZZER, 0);
  isSerial = 3; // on passe la valeur de isSerial a 3 avant de revenir dans la loop pour permettre la lecture du message
}

/**
  *\brief fonction qui permet d'afficher le message a l'ecran et qui donne les instructions pour en sortir
  *\ cette fonction prend en parametre un tableau de charactere correspondant au message que l'on souhaite afficher
  */
void affichMessage(char msg[]){
  int i = 0; // initialise la variable i qui est un entier initialise a la valeur 0
  // On passe la couleur du fond en blanc ainsi que la couleur de fond du texte
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setCursor(0,10); // on place un curseur en haut a gauche de l'ecran et on y ecrit notre message avec la fonction print qui permet de faire des retours a la ligne automatique si on arrive au bout de l'ecran
  tft.print(msg);
  tft.drawString("Down pour sortir",10,200); // On ecrit en bas de l'ecran l'instruction pour sortir 

  // on lance une boucle qui va durer 30 secondes et qui va permettre a l'utilisateur de quitter par lui meme l'ecran de message
  while(i<3000){
    // si pendant ces 30 secondes l'utilisateur apui sur joystick vers le bas alors on rappelle la fonction display afin de reafficher l'ecran de base et donc de quitter celui des messages
    if (digitalRead(WIO_5S_DOWN) == LOW){ 
      if(affluence < 11){display(0);} // Si l'affluence est inferieur a 11 alors, on rappelle la fonction avec le parametre 0 avec quelle mette le fond en vert
      else{display(1);} // Si l'affluence est superieur a 11 alors, on rappelle la fonction avec le parametre 1 avec quelle mette le fond en rouge 
      isSerial=0; // on met la variable isSerial a 0 avant de quitter l'ecran. Cela signifie qu'on pourra rouvrir l'ecran des message afin de le reafficher
      return; // on quitte la fonction car on a plus besoin du chronometre 
    }
    i+=10;      // gestion du delai qui dure 30 secondes et qui permet a l'utilisateur de faire des actions pendant celles ci
    delay(100); 
  }
  // Si au bout des 30 secondes l'utilisateur n'a rien fait alors on affiche l'ecran de base avec la fonction display et on quitte le fonction.
  if(affluence < 11){display(0);}
  else{display(1);}
  isSerial=0;
}

///////////////
//// STATS ////
///////////////

/**
  *\brief fonction permettant d'ajouter 1 aux compteurs
  *\ elle prend en parametre un booleen isRemiseAZero qui nous permettra de changer l'affichage
  */
void incrementation(bool isRemiseAZero){
  // augmente les valeurs de visiteurs et de affluence de 1 
  visiteurs = visiteurs+1;
  affluence = affluence+1;
  // si l'affluence est egale a 11, il faut donc changer la couleur du fond en rouge
  if(affluence==11){
    // Si on avait avant de cliquer sur "+" cliquer sur "RAZ" alors, il faut laisser le message "Confirmez ?" afficher a l'ecran. C'est ce que la variable isRemiseAZero permet de faire
    if(isRemiseAZero==false){ // Si isRemiseAZero est false alors on appelle la fonction display avec le parametre 1 qui n'affiche pas de texte supplemetaire
      display(1);
      analogWrite(WIO_BUZZER, 128); // On actionne le buzzer pour indiquer que l'affluence max est depasse
      delay(200);
      analogWrite(WIO_BUZZER, 0);
      return; // On quitte la fonction car l'affichage a bien ete effectue
    }
    // Si isRemiseAZero est true alors on appelle la fonction display avec le parametre 4 afin qu'il affiche "Confirmez ?" en bas de l'ecran
    else{
      display(4);  
      analogWrite(WIO_BUZZER, 128);
      delay(200);
      analogWrite(WIO_BUZZER, 0);
      return;
    }
  }
  // Si on est pas passe de 10 a 11 en affluence alors on appelle la fonction affichResults afin d'afficher les nouvelles valeur de visiteurs et d'affluence
  affichResults();
  delay(200);    
}

/**
  *\brief fonction permettant d'enlever 1 au compteur
  *\ elle prend en parametre un booleen isRemiseAZero qui nous permettra de changer l'affichage
  */
void decrementation(bool isRemiseAZero){
  if(dataVerif()){ // on appelle la fonction dataVerif pour verifier que affluence ne descende pas en dessous de 0
      affluence = affluence-1; // on enleve 1 a affluence 
    }
    // Si l'affluence passe de 11 a 10 alors il faut changer la couleur de fond en vert
    if(affluence==10){
    // comme pour la fonction incrementation, on verifie si on a besoin d'ecrire le message de confirmation de la remise a 0 grace a la variable isRemiseAZero
      if(isRemiseAZero==false){ 
        display(0); // On appelle la fonction display avec different paremetre afin de gerer l'affichage
      }
      else{
        display(3);  
      }  
    }
    // Si on est pas passe de 11 a 10 alors on appelle la fonction affichResults afin d'afficher la nouvelle valeur de affluence
    affichResults();
    delay(200); 
}

/**
  *\brief fonction qui permet de verifier que la valeur de affluence ne descend pas en dessous de 0
  *\Retourne un booleen en fonction de si oui ou non l'affluence est egal a 0
  */
bool dataVerif(){
  if(affluence == 0){return false;}
  return true;
}

/**
  *\brief fonction qui permet de gerer la remise a zero des compteurs
  */
void RemiseAZero(){
  int i = 0;
  tft.drawString("Confirmez ?",110,200); // quand on appelle la fonction on ecrit "Confirmez ?" en bas de l'ecran
  // En attendant la confirmation de l'utilisateur on lance une boucle qui va durer 5 secondes pendant lesquels l'utilisateur aura plusieurs action possible
  while(i < 500){
    // si il presse le bouton "+" alors, on appellera la fonction incrementation
    if (digitalRead(WIO_KEY_C) == LOW){
      incrementation(true); // on appelle incrementation avec le parametre true afin de gerer l'affichage du texte "Confirmez ?"
      // etant donne que dans la fonction incrementation on trouve un delay de 200ms, on ajoute 20 a i afin de compter le temps qui passe et on retourne directement au debut de la boucle
      i+=20; 
      continue;
    }
    // si il presse le bouton "-" alors, on appelle la fonction decrementation
    if (digitalRead(WIO_KEY_B) == LOW){
      decrementation(true);
      // meme fonctionnement que pour le bouton "+"
      i+=20;
      continue;
    }
    // si il presse le bouton du joystick alors il comfirme la remise a zero
    if(digitalRead(WIO_5S_PRESS) == LOW){
      // on passe les valeurs de visiteurs et de affluence a 0
      visiteurs=0;
      affluence=0;
      // on reaffiche l'ecran de base avec les nouvelles valeurs
      display(0);
      // On ecrit dans le port serie un message pour notifier la remise a zero
      Serial.println("-------------");
      Serial.println("Remise des compteurs à zéro !");
      return; // si l'utilisateur a valider alors, on retourne a la loop
    }
    // gestion de l'ecoulement du temps 
    i+=10;
    delay(100);
  }
  // Si au bout des 5 secondes l'utilisatuer n'a pas confirmer la remise a 0 alors on recouvre le message avec un rectangle dont la couleur depend de la couleur du fond et donc de l'effluence
  if(affluence  < 11){
    tft.fillRect(110,200,200,50,TFT_GREEN);
  }
  else{
     tft.fillRect(110,200,200,50,TFT_RED);
  }
}

/**
  *\brief permet d'envoyer les statistiques actuelles toutes les 15 minutes
  */
void envoyerLesStats(){
  Serial.println(); // saut de ligne
  DispTimeSerial(); // apelle la fonction d'affichage de l'heure dans le moniteur de série
  // affichage des statistiques dans le moniteur de série :
  Serial.print("Visiteurs totales : ");
  Serial.println(visiteurs,DEC);
  Serial.print("Affluence actuelle : ");
  Serial.println(affluence,DEC);
  Serial.println("-------------");
}
///////////////
//// SETUP ////
///////////////

void setup() {
  tft.begin(); // lancer tft
  rtc.begin(); // lancer rtc
  Serial.begin(115200); // communiaction avec le moniteur de série en 115200 baud 
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // on ajuste rtc sur l'heure de compilation pour que le Wio soit autonome sur l'heure 
                                                  //(lors de chaque 'reset' l'heure que contient 'rtc' sera donc remise à zéros sur l'heure de compilation).
  needSerialCom(); // lance la fonction qui vérifie que le moniteur série est lancé
  DispTimeSerial(); // affiche l'heure de début sur le moniteur de série

  // On parametre les boutons qui nous seront necessaire
  pinMode(WIO_KEY_A, INPUT_PULLUP); // bouton a du wio terminal en input
  pinMode(WIO_KEY_B, INPUT_PULLUP); // bouton b du wio terminal en input 
  pinMode(WIO_KEY_C, INPUT_PULLUP); // bouton c du wio terminal en input
  pinMode(WIO_5S_PRESS, INPUT_PULLUP); // bouton lorsqu'on appui sur le joystick du wio terminal en input
  pinMode(WIO_5S_UP, INPUT_PULLUP); // joystick vers le haut du wio terminal en input
  pinMode(WIO_5S_DOWN, INPUT_PULLUP); // joystick vers le bas du wio terminal en input
  display(0); // affichage de base
}

//////////////
//// LOOP ////
//////////////

/**
  *\brief fonction de base qui attendra les actions de l'utilisateurs afin d'appeler les bonnes fonctions correspondantes
  */
void loop() {

  // on regarde sur le port serie si un message est envoye par l'utilisateur. Si c'est le cas alors, on agrde sa taille dans la variable charDispo
  int charDispo = Serial.available();
  // Si un message est lu
  if(charDispo > 0){
    int i = 0;
    // on allour dynamiquement la place necessaire pour y inclure le message a un tableau de char
    tabChar = new char[charDispo];
    // pour chaque lettre lu dans le message du port serie on la palce dans una case de notre tableau
    while(charDispo > 0)
    {
        char charlu = Serial.read(); // on place le premier caractere lu dans la variable charLu
        tabChar[i] = charlu; // on place ce char dans la bonne case de notre tableau
        charDispo = Serial.available();
        i = i + 1;
        isSerial = 1; // on passe la valeur de isSerial a 1 afin d'afficher le nouveau message
    }
  }
  
  // Si isSerial est egal a 1 alors, on appelle la fonction newMessage afin d'ecrire sur l'ecran qu'un nouveau message est arrive
  if (isSerial == 1){
    newMessage();  
  }

  // Si isSerial est different de 2 (la valeur qu'on lui a attribue de base) alors, il est possible de lire le message
  if (isSerial != 2){
    // si le joystick est pousse vers le haut, on appelle la fonction affichMessage avec en parametre notre texte dans un tableau afin de l'afficher
    if (digitalRead(WIO_5S_UP) == LOW){  
      affichMessage(tabChar);  
    }  
  }

  now=rtc.now(); // mise a jour de la variable now 
  
  if(tmp.minute()!=now.minute()||tmp.hour()!=now.hour()){ // vérifie si le dernier horaire est dépassé.
    dispTime();
    if((now.minute()%15)==0){ // si le temps a changé, on regarde si les minutes ne sont pas multiple de 15 pour envoyer les statistiques actuelles.
      envoyerLesStats();
    }
  }

  // Si l'utilisateur appui sur le bouton "+" alors, on appelle la fonction incrementation
  if (digitalRead(WIO_KEY_C) == LOW){
    incrementation(false);
  }
  // Si l'utilisateur appui sur le bouton "-" alors, on appelle la fonction decrementation
  if (digitalRead(WIO_KEY_B) == LOW){
    decrementation(false);
  }
  // Si l'utilisateur appui sur le bouton "RAZ" alors, on appelle la fonction RemisAZero
  if (digitalRead(WIO_KEY_A) == LOW){
   RemiseAZero();
  }
}
