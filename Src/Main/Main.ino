#include <LiquidCrystal.h>
#include <avr/sleep.h>
#include <stdlib.h>

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

const int debounceDelay = 50; // Ritardo per debounce in millisecondi
unsigned long lastDebounceTime1 = 0; //ultimo click del bottone 1
unsigned long lastDebounceTime2 = 0; //ultimo click del bottone 2
unsigned long lastDebounceTime3 = 0; //ultimo click del bottone 3
unsigned long lastDebounceTime4 = 0; //ultimo click del bottone 4
bool lastButtonState1 = HIGH;  // Stato precedente del bottone 1
bool lastButtonState2 = HIGH;  // Stato precedente del bottone 2
bool lastButtonState3 = HIGH;  // Stato precedente del bottone 3
bool lastButtonState4 = HIGH;  // Stato precedente del bottone 4

int fadeAmount;
int currIntensity;
int buttonState;
int score;
int numero;
char binarioString[5];
unsigned long startMillis;
unsigned long currentMillis;
int timelimit;
int difficulty;
int ledState[] = {0,0,0,0};
int gameInitialized = 0;
bool gameActive = false;
const int ledPins[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};    // L1, L2, L3, L4 (binary LEDs)
const int buttonPins[] = {BUT1_PIN, BUT2_PIN, BUT3_PIN, BUT4_PIN}; // B1, B2, B3, B4 (buttons for binary LEDs)

// Create an LCD object. Parameters: (RS, E, D4, D5, D6, D7):
LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);

int generateRandomicNumber()
{
  return random(0, 16); // random(min, max) generate and return a number between min and max-1
}

// Function to reset LEDs
void resetLeds()
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(ledPins[i], LOW);
    ledState[i] = LOW;
  }
  bool lastButtonState1 = HIGH;  
  bool lastButtonState2 = HIGH;  
  bool lastButtonState3 = HIGH; 
  bool lastButtonState4 = HIGH; 
}

void wakeUpNow() {}

void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW
  sleep_mode();
  sleep_disable();
  detachInterrupt(0);
}

void setup()
{
  // Initialization to generate random number between 0 and 15
  Serial.begin(9600);
  randomSeed(analogRead(0));
  lcd.begin(16, 2);
  // Setup pins
  for (int i = 0; i < 4; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(LEDS_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  score = 0;

  fadeAmount = 10;
  currIntensity = 0;
  
  lcd.setCursor(0, 0);
  lcd.write("Welcome to GMB!");
  lcd.setCursor(0, 1);
  lcd.write("Press B1 to Strt");
}

// Function to check if binary is correct
bool checkBinary()
{
  for (int i = 0; i < 4; i++)
  {
    if (ledState[i] != ((numero >> i) & 1))
    {
      return false;
    }
  }
  return true;
}

void initGame()
{
    unsigned long currentTime = millis();
    analogWrite(LEDS_PIN, currIntensity);
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255)
    {
      fadeAmount = -fadeAmount;
    }


      // Bottone 1
    bool currentButtonState1 = digitalRead(BUT1_PIN);
    if (currentButtonState1 != lastButtonState1 && currentButtonState1 == HIGH && (currentTime - lastDebounceTime1 > debounceDelay)) {
      lcd.clear();
      gameInitialized = 1;
      analogWrite(LEDS_PIN, 0);
      resetLeds();
      Serial.write("bt1 click");
      lastDebounceTime1 = currentTime;
    }
    lastButtonState1 = currentButtonState1;
    
    delay(20);
  
}

void loop()
{
  switch (gameInitialized)
  {
  case 0:
    initGame();
    break;
  case 1:
    //gameLoop();
    ledHandler();
    break;
  }
}

void gameLoop()
{
  int potRead = analogRead(POT_PIN);

  if (!gameActive)
  {
    if (digitalRead(BUT1_PIN) == LOW)
    {
      gameActive = true;
      resetLeds();
      lcd.setCursor(0, 0);
      lcd.write("Go!");
      difficulty = map(potRead, 0, 1023, 1, 4); // Map potentiometer value to difficulty
      timelimit = 5000 / difficulty;            // Adjust time based on difficulty
      delay(2000);
    }
  }
  else
  {
    numero = generateRandomicNumber();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("Number:%d ", numero);

    long startTime = millis();
    bool correct = false;

    while (millis() - startTime < timelimit)
    {
      for (int i = 0; i < 4; i++)
      {
        if (digitalRead(buttonPins[i]) == LOW)
        {
          ledState[i] = !ledState[i];
          digitalWrite(ledPins[i], ledState[i]);
          delay(200); // Debounce delay
        }
      }

      if (checkBinary())
      {
        correct = true;
        break;
      }
    }

    if (correct)
    {
      score++;
      lcd.write("GOOD! Score:%d ", score);
      delay(2000);
    }
    else
    {
      digitalWrite(LEDS_PIN, HIGH);
      delay(1000);
      digitalWrite(LEDS_PIN, LOW);

      lcd.write("Game Over - Final Score:%d ", score);
      delay(5000);
      gameActive = false;
    }

    // Reduce time for the next round
    timelimit = timelimit - timelimit * 0.1;
  }
}

void ledHandler() {
  unsigned long currentTime = millis();

  // Bottone 1
  bool currentButtonState1 = digitalRead(BUT1_PIN);
  if (currentButtonState1 != lastButtonState1 && currentButtonState1 == HIGH && (currentTime - lastDebounceTime1 > debounceDelay)) {
    ledState[0] = reverseValue(ledState[0]);
    Serial.write("bt1 click");
    lastDebounceTime1 = currentTime;
  }
  lastButtonState1 = currentButtonState1;

  // Bottone 2
  bool currentButtonState2 = digitalRead(BUT2_PIN);
  if (currentButtonState2 != lastButtonState2 && currentButtonState2 == HIGH && (currentTime - lastDebounceTime2 > debounceDelay)) {
    ledState[1] = reverseValue(ledState[1]);
    Serial.write("bt2 click");
    lastDebounceTime2 = currentTime;
  }
  lastButtonState2 = currentButtonState2;

  // Bottone 3
  bool currentButtonState3 = digitalRead(BUT3_PIN);
  if (currentButtonState3 != lastButtonState3 && currentButtonState3 == HIGH && (currentTime - lastDebounceTime3 > debounceDelay)) {
    ledState[2] = reverseValue(ledState[2]);
    Serial.write("bt3 click");
    lastDebounceTime3 = currentTime;
  }
  lastButtonState3 = currentButtonState3;

  // Bottone 4
  bool currentButtonState4 = digitalRead(BUT4_PIN);
  if (currentButtonState4 != lastButtonState4 && currentButtonState4 == HIGH && (currentTime - lastDebounceTime4 > debounceDelay)) {
    ledState[3] = reverseValue(ledState[3]);
    Serial.write("bt4 click");
    lastDebounceTime4 = currentTime;
  }
  lastButtonState4 = currentButtonState4;

  delay(20);
  Serial.println("button state: " + (String)ledState[0] + " " + (String)ledState[1] + " " + (String)ledState[2] + " " + (String)ledState[3]);
  ledUpdater();
}

void ledUpdater() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], ledState[i]);
  }
}

int reverseValue(int i) {
  return i == 0 ? 1 : 0;
}