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
}

void loop() {
  // How to use generateRandomicNumber
  // int numero = generateRandomicNumber();
  
}

void initGame() {
  
  int fadeAmount = 10;
  int currIntensity = 0;
  int buttonState = digitalRead(BUT1_PIN);
  lcd.setCursor(0, 0);
  lcd.write("Welcome to GMB!");
  lcd.setCursor(0, 1);
  lcd.write("Press B1 to Strt");
  bool endInit = true;
  while(endInit){
    analogWrite(LEDS_PIN, currIntensity);   
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255) {
      fadeAmount = -fadeAmount ; 
    }     

    if (buttonState == HIGH)  {
      endInit = false;
    }
    delay(20); 
  }
}
