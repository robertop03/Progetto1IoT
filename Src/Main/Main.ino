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

int score;
int numero;
char binarioString[5];
unsigned long startMillis;
unsigned long currentMillis;
int timelimit;
int difficulty;
int ledState[4];
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
  initGame();
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
  int fadeAmount = 10;
  int currIntensity = 0;
  int buttonState = digitalRead(BUT1_PIN);
  lcd.setCursor(0, 0);
  lcd.write("Welcome to GMB!");
  lcd.setCursor(0, 1);
  lcd.write("Press B1 to Strt");
  bool endInit = true;
  while (endInit)
  {
    analogWrite(LEDS_PIN, currIntensity);
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255)
    {
      fadeAmount = -fadeAmount;
    }

    if (buttonState == HIGH)
    {
      endInit = false;
      lcd.clear();
      game();
    }
    delay(20);
  }
}

void game()
{
  int but1 = digitalRead(BUT1_PIN);
  int but2 = digitalRead(BUT2_PIN);
  int but3 = digitalRead(BUT3_PIN);
  int but4 = digitalRead(BUT4_PIN);
  int potRead = analogRead(POT_PIN);
  bool gameActive = false;

  if (!gameActive)
  {
    if (digitalRead(but1) == LOW)
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
