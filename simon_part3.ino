
#include <SPI.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Esplora.h>
#include <TFT.h> 

char pt[4];
// Definicions dels pins
const int green = 10;
const int blue = 9;
const int red = 5;
int segons = 0;

boolean timer = false;
int jugada = 4;

boolean encert;

int c = 0;
const int MAX = 20; //Aqui podem configurar la llargada maxima de la secuencia
int joc[MAX];

int ran;

void setup() {                
  //Configurar la placa amb entrades i sortides
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(A4,INPUT);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  //Configurar por serie per si cal fer alguna comprovacio
  Serial.begin(9600);
  randomSeed(analogRead(0));//seed del random, el pin 0 no esta conectat, el "soroll" que hi haura doncs, permetra un random bo
  
  cli();          // deshabilitar interrupcions globals
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B

  // count del timer
  OCR1A = 15624;

  //habilitar el mode CTC
  TCCR1B |= (1 << WGM12);

  //  CS10 && CS12 bits 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);

  // habilitar la interrupcio "timer compare"
  TIMSK1 |= (1 << OCIE1A);
  pinMode(11, INPUT); //11 pin d'interrupcions INT0
  attachInterrupt(0, perdre, HIGH);
  // habilitar interrupcions
  sei();

  Iniciar();
}

bool top() {
  digitalWrite(A0, LOW);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

  //int topval = Esplora.readJoystickY();

  
  return analogRead(A4)==LOW ;//actiu a nivell baix

}

bool down() {
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  //int downval = Esplora.readJoystickY();
  return analogRead(A4)==LOW ;//actiu a nivell baix
  
}

bool left() {
  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

  //int leftval = Esplora.readJoystickX();
  return analogRead(A4)==LOW ;//actiu a nivell baix
}

bool right() {
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

  //int rightval = Esplora.readJoystickX();
  return analogRead(A4)==LOW ;//actiu a nivell baix
}

void vermell() {
  digitalWrite(red, HIGH);
  delay(1000);
  digitalWrite(red, LOW);


  EsploraTFT.fill(255, 0, 0); //RED
  EsploraTFT.circle(120,55,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(20,55,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,10,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,100,20); 
  
  Esplora.tone(440, 10);
  
}

void verd() {
  digitalWrite(green, HIGH);
  delay(1000);
  digitalWrite(green, LOW);

  
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,100,20);
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(20,55,20); 
  EsploraTFT.fill(0,255,0); //GREEN
  EsploraTFT.circle(70,10,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(120,55,20); 
  Esplora.tone(880, 10);
  
}

void blau() {
  digitalWrite(blue, HIGH);
  delay(1000);
  digitalWrite(blue, LOW);

  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,100,20);  
  EsploraTFT.fill(0, 0, 255); //BLUE
  EsploraTFT.circle(20,55,20);  
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,10,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(120,55,20); 
  Esplora.tone(220, 10);
  
}

void groc() {
  digitalWrite(green, HIGH);
  digitalWrite(red, HIGH);
  delay(1000);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  

  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(120,55,20);
      
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(20,55,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,10,20); 
  EsploraTFT.fill(255, 255, 0); //YELLOW
  EsploraTFT.circle(70,100,20);
  Esplora.tone(110, 10);

}

void mostrarNivell(){
  EsploraTFT.setTextSize(2);
  String level=String(c);
  level.toCharArray(pt,4);
  EsploraTFT.text(pt, 75, 75);
}

void mostrarSecuencia(int limit){

  EsploraTFT.background(0,200,0);
  EsploraTFT.setTextSize(1);
  EsploraTFT.text(" Nivell:", 46, 55);
  mostrarNivell();
  //Mostrem la secuencia guardada fins el moment
  delay(500);
  for(int i=0;i<=limit;i++){
    if(joc[i]==0){
      verd();
      delay(500); // hi ha un petit delay per poder visualitzar be la secuencia
    }
    else if(joc[i]==1){
      blau();
      delay(500);
    }
    else if(joc[i]==2){
      vermell();
      delay(500);
    }
    else if(joc[i]==3){
      groc();
      delay(500);
    }
  }
  EsploraTFT.background(0,0,0);

  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(120,55,20);
      
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(20,55,20); 
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.circle(70,10,20); 
  EsploraTFT.fill(255, 255, 255); //YELLOW
  EsploraTFT.circle(70,100,20);
  
}

boolean botons(){
  return top() || down() || right() || left();
}

int Boto() {
  while (!botons()) {
    delay(50);
  }
  while (!botons()) {
    delay(50);
  }
  return !botons();
}

void Iniciar(){
  EsploraTFT.begin();
  EsploraTFT.background(0,0,0);
  EsploraTFT.stroke(200,20,180); // set the stroke color to red 
  EsploraTFT.setTextSize(2);
  EsploraTFT.text("ArduSimo", 25, 20);
  EsploraTFT.text("Pau Dani", 25, 45);
  EsploraTFT.stroke(200,20,180); // set the stroke color to red 
  EsploraTFT.setTextSize(1);
  EsploraTFT.text("Apreta un boto per", 30, 100);
  EsploraTFT.text("iniciar", 55, 110);
  Boto();
}


void resetJoc(){
  //Fem un reset del joc, el comptador queda a zero i mostrem una secuencia de leds vermells
  c = 0;
  segons = 0;
  vermell();
  delay(500);
  vermell();
  delay(500);
  vermell();
  delay(500);
  Iniciar();
}

void victoriaFinal(){
  //Mostrem secuencia de 3 leds verds amb poc delay per indicar clarament que s'ha guanyat
  verd();
  delay(250);
  verd();
  delay(250);
  verd();
  delay(250);
}

void perdre(){//simplement ens asegurem que encert es fals cosa que fara saltar el reset
  // a utilitzar amb les interrupcions
  encert = false;
}

ISR(TIMER1_COMPA_vect){
  //no es pot programar un temporitzador "natural" a 30 segons
  //per a interrompre cada 30 segons, produiria overflow
  //pero es pot programar un temporitzador cada segon i anar sumant!
  if(timer){//si el temporitzador esta activat
    segons++;//sumem un segon
    if(segons == 30){//si arriba a 30
      if(jugada == 4){//i encara tenim la tecla per defecte
        perdre();//automaticament encert pasa a fals(ens fara perdre)
      }
      segons = 0;//reiniciem segons
    }
  }
  else{
    segons = 0;//si no tenim el temporitzador per si decas ens asegurem que segons es 0
    //tecnicament tampoc caldria posar aixo aqui
  }
}

//0 VERD, 1 BLAU, 2 VERRMELL, 3 GROC
void loop(){
  segons = 0;
  //randomSeed(millis()); seria una altre manera de fer random 
  ran = random(4); //random de 0 a 3
  joc[c]=ran; //afegim la jugada
  mostrarSecuencia(c); //mostrem la secuencia fins al moment
  
  int i = 0;
  encert = true; //partim de que s'ha encertat
  while(i <= c && encert){ //Es un esquema de cerca! Se surt quan es falla
    jugada = 4; //inicialitzem a 4 ja que per exemple a 4 no hi tenim cap color
    while(jugada==4 && encert){
      timer = true;//engegem el temporitzador
      if(top()){
        while(top()){//mentre es premi el boto es mostrara el color aixi l'usuari "te temps per jugar"
          verd();
        }
        jugada = 0;
      }
      else if(left()){
        while(left()){
          blau();
        }
        jugada = 1;
      }
      else if(right()){
        while(right()){
          vermell();
        }
        jugada = 2;
      }
      else if(down()){
        while(down()){
          groc();
        }
        jugada = 3;
      }
    }
    timer = false;//apaguem el temporitzador perque no molesti
    if(joc[i]!=jugada){//si no coincideix encert pasa a fals i se surt de la "partida"
      perdre();
    }
    else{//si s'ha encertat, seguim comprovant el seguent
      i++;
    }
  }
  
  if(!encert){//Si s'ha fallat o hem arribat al maxim reiniciem el joc
    resetJoc();
  }
  else if(c==MAX){
    victoriaFinal();
    resetJoc();
  }
  else{//Si s'ha encertat la secuencia afegim un element mes!
    c++;
  }
  
  
}

