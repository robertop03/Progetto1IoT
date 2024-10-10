#include <LiquidCrystal.h>
#include <avr/sleep.h>


#define LED1_PIN 10
#define LED2_PIN 11
#define LED3_PIN 12 
#define LED4_PIN 13
#define LEDS_PIN 9
#define BUT1_PIN A1
#define BUT2_PIN A2
#define BUT3_PIN A3
#define BUT4_PIN A4
#define POT_PIN A5

int score;

bool led1;
bool led2;
bool led3;
bool led4;


// Create an LCD object. Parameters: (RS, E, D4, D5, D6, D7):
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);


int generateRandomicNumber() {
  return random(0, 16);  // random(min, max) generate and return a number between min and max-1
}

void wakeUpNow(){}

void sleepNow(){
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW
  sleep_mode();
  sleep_disable();
  detachInterrupt(0);
}

void setup() {
  // Initialization to generate random number between 0 and 15
  Serial.begin(9600);
  randomSeed(analogRead(0));
  lcd.begin(16, 2);
  score = 0;
  initGame();

  pinMode(BUT1_PIN, INPUT);
  pinMode(BUT2_PIN, INPUT);
  pinMode(BUT3_PIN, INPUT);
  pinMode(BUT4_PIN, INPUT);


  led1 = false;
  led2 = false;
  led3 = false;
  led4 = false;
}

void loop() {
  // How to use generateRandomicNumber
  // int numero = generateRandomicNumber();

  ledHandler();

  delay(20); 
}

void initGame() {
  
  int fadeAmount = 5;
  int currIntensity = 0;
  int buttonState;
  lcd.setCursor(0, 0);
  lcd.write("Welcome to GMB!");
  lcd.setCursor(0, 1);
  lcd.write("Press B1 to Strt");

  while(1){
    buttonState = digitalRead(BUT1_PIN);
    analogWrite(LEDS_PIN, currIntensity);   
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255) {
      fadeAmount = -fadeAmount ; 
    }     

    if (buttonState == LOW)  {
      break;
    }

    delay(20); 

  }


}

void ledHandler(){


  if(digitalRead(BUT1_PIN)==LOW){
    led1 = !led1;
    Serial.write("bt1 click");
    while(digitalRead(BUT1_PIN)==LOW){};
  }

  if(digitalRead(BUT2_PIN)==LOW){
    led2 = !led2;
    Serial.write("bt2 click");
    while(digitalRead(BUT2_PIN)==LOW){};
  }

  if(digitalRead(BUT3_PIN)==LOW){
    led3 = !led3;
    Serial.write("bt3 click");
    while(digitalRead(BUT3_PIN)==LOW){};
  }

  if(digitalRead(BUT4_PIN)==LOW){
    led4 = !led4;
    Serial.write("bt4 click");
    while(digitalRead(BUT4_PIN)==LOW){};
  }

  delay(20);

  ledUpdater();
  
}

void ledUpdater(){
  digitalWrite(LED1_PIN,led1);
  digitalWrite(LED2_PIN,led2);
  digitalWrite(LED3_PIN,led3);
  digitalWrite(LED4_PIN,led4);
}
