#include <LiquidCrystal.h>
#include <avr/sleep.h>
#include <stdlib.h>

#define LED1_PIN A1
#define LED2_PIN A2
#define LED3_PIN A3
#define LED4_PIN A4
#define LEDS_PIN 9
#define BUT1_PIN 8
#define BUT2_PIN 10
#define BUT3_PIN 11
#define BUT4_PIN 12
#define POT_PIN A5

const int debounceDelay = 50;        // Ritardo per debounce in millisecondi
unsigned long lastDebounceTime1 = 0; // ultimo click del bottone 1
unsigned long lastDebounceTime2 = 0; // ultimo click del bottone 2
unsigned long lastDebounceTime3 = 0; // ultimo click del bottone 3
unsigned long lastDebounceTime4 = 0; // ultimo click del bottone 4
bool lastButtonState1 = HIGH;        // Stato precedente del bottone 1
bool lastButtonState2 = HIGH;        // Stato precedente del bottone 2
bool lastButtonState3 = HIGH;        // Stato precedente del bottone 3
bool lastButtonState4 = HIGH;        // Stato precedente del bottone 4

int fadeAmount;
int currIntensity;
int buttonState;
int score;
int numero;
char binarioString[5];
unsigned long startMillis;
unsigned long currentMillis;
int timelimit;
int timeout = 10000;
int difficulty = 1;
float factorF = 1.0; // fattore impostato con il potenziometro
float factorL = 0.9; // fattore che scala il tempo ogni livello completato
int ledState[] = {0, 0, 0, 0};
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
  lastButtonState1 = HIGH;
  lastButtonState2 = HIGH;
  lastButtonState3 = HIGH;
  lastButtonState4 = HIGH;
}

void readDifficultyLevel()
{
  int potValue = analogRead(POT_PIN);        // Leggi il valore del potenziometro (0-1023)
  difficulty = map(potValue, 0, 1023, 1, 4); // Mappa il valore su un range 1-4
  factorF = 1.0 + (difficulty - 1) * 0.5;    // F è 1.0 per livello 1, 2.5 per livello 4

  // Mostra il livello di difficoltà sul display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Difficulty: L");
  lcd.print(difficulty);
  delay(1000); // Piccola attesa per visualizzare il livello
}

void wakeUpNow() {}

void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Imposta la modalità di sleep
  sleep_enable();                      // Abilita la modalità di sleep

  // Imposta interrupt per svegliare Arduino quando un qualsiasi pulsante viene premuto
  attachInterrupt(digitalPinToInterrupt(8), wakeUpNow, LOW);
  attachInterrupt(digitalPinToInterrupt(10), wakeUpNow, LOW);
  attachInterrupt(digitalPinToInterrupt(11), wakeUpNow, LOW);
  attachInterrupt(digitalPinToInterrupt(12), wakeUpNow, LOW);

  sleep_mode(); // Arduino va in modalità sleep

  // Quando Arduino si risveglia:
  sleep_disable();                           // Disabilita la modalità di sleep
  detachInterrupt(digitalPinToInterrupt(8)); // Rimuove gli interrupt
  detachInterrupt(digitalPinToInterrupt(10));
  detachInterrupt(digitalPinToInterrupt(11));
  detachInterrupt(digitalPinToInterrupt(12));
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
  startMillis = millis();
}

// Function to check if binary is correct
bool checkBinary()
{
  Serial.print("Numero generato (binario): ");
  Serial.println(numero, BIN); // Visualizza il numero generato in binario

  Serial.print("Stato dei LED: ");
  for (int i = 0; i < 4; i++)
  {
    Serial.print(ledState[i]); // Mostra lo stato dei LED
  }
  Serial.println();

  bool isCorrect = true; // Assumiamo che sia corretto

  // Confronto bit per bit (inverti ordine se necessario)
  for (int i = 0; i < 4; i++)
  {
    int bitExpected = (numero >> (3 - i)) & 1; // Cambiato l'ordine dei bit
    int ledBit = ledState[i];                  // Stato corrente del LED
    // Se c'è un errore nel confronto, settiamo isCorrect a false
    if (ledBit != bitExpected)
    {
      isCorrect = false;
    }
  }

  return isCorrect; // Restituisce true solo se tutti i bit corrispondono
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

  bool currentButtonState1 = digitalRead(BUT1_PIN);

  // Controllo se è trascorso il timeout e nessun pulsante è stato premuto
  if (currentTime - startMillis >= timeout && currentButtonState1 == lastButtonState1)
  {
    sleepNow(); // Manda Arduino in modalità sleep
  }

  // Se il bottone 1 viene premuto, inizializza il gioco
  if (currentButtonState1 != lastButtonState1 && currentButtonState1 == HIGH && (currentTime - lastDebounceTime1 > debounceDelay))
  {
    lcd.clear();
    readDifficultyLevel();
    gameInitialized = 1; // Imposta il gioco come inizializzato
    lcd.setCursor(0, 0);
    lcd.print("Go!");  // Mostra "Go!" sul display
    delay(500);        // Attesa per visualizzare "Go!"
    gameActive = true; // Attiva il gioco
    resetLeds();       // Resetta i LED
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
    gameLoop();
    break;
  }
}

void gameLoop()
{
  int potRead = analogRead(POT_PIN);

  if (!gameActive)
  {
    // Questo caso non dovrebbe mai accadere poiché il gioco è già attivo
    return;
  }

  // Solo se il gioco è attivo, generiamo un numero casuale
  numero = generateRandomicNumber();
  digitalWrite(LEDS_PIN, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Number: ");
  lcd.print(numero);
  delay(500);

  long startTime = millis();
  bool correct = false;

  // Ciclo di attesa fino al termine del tempo
  timelimit = (int)(10000 / factorF);
  while (millis() - startTime < timelimit)
  {
    ledHandler(); // Gestisce gli input dei pulsanti e gli LED
  }

  // Controllo se il numero binario è corretto dopo che il tempo è scaduto
  if (checkBinary()) // Controlla se l'input in binario è corretto
  {
    correct = true;
  }
  if (correct)
  {
    Serial.println("Boolean: True"); // Se true, stampa "True"
  }
  else
  {
    Serial.println("Boolean: False"); // Se false, stampa "False"
  }

  if (correct)
  {
    score++; // Aumenta il punteggio se la risposta è corretta
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GOOD! Score: ");
    lcd.print(score);
    delay(2000);
    timelimit = (int)(timelimit * factorL);
  }
  else
  {
    digitalWrite(LEDS_PIN, HIGH); // Accende il LED rosso
    delay(1000);                  // Tieni acceso per 1 secondo
    digitalWrite(LEDS_PIN, LOW);  // Spegni il LED rosso

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over -");
    lcd.setCursor(0, 1);
    lcd.print("Final Score: ");
    lcd.print(score); // Mostra il punteggio finale
    delay(10000);
    // Reset game variables
    score = 0;           // Resetta il punteggio
    gameActive = false;  // Resetta il gioco
    gameInitialized = 0; // Riporta il gioco allo stato iniziale
    resetLeds();         // Resetta i LED
    lcd.clear();         // Pulisce lo schermo
    lcd.setCursor(0, 0);
    lcd.print("Welcome to GMB!"); // Mostra di nuovo il messaggio di benvenuto
    lcd.setCursor(0, 1);
    lcd.print("Press B1 to Strt"); // Istruzioni per l'inizio
  }
}

void ledHandler()
{
  unsigned long currentTime = millis();

  // Bottone 1
  bool currentButtonState1 = digitalRead(BUT1_PIN);
  if (currentButtonState1 != lastButtonState1 && currentButtonState1 == HIGH && (currentTime - lastDebounceTime1 > debounceDelay))
  {
    ledState[0] = reverseValue(ledState[0]);
    Serial.write("bt1 click");
    lastDebounceTime1 = currentTime;
  }
  lastButtonState1 = currentButtonState1;

  // Bottone 2
  bool currentButtonState2 = digitalRead(BUT2_PIN);
  if (currentButtonState2 != lastButtonState2 && currentButtonState2 == HIGH && (currentTime - lastDebounceTime2 > debounceDelay))
  {
    ledState[1] = reverseValue(ledState[1]);
    Serial.write("bt2 click");
    lastDebounceTime2 = currentTime;
  }
  lastButtonState2 = currentButtonState2;

  // Bottone 3
  bool currentButtonState3 = digitalRead(BUT3_PIN);
  if (currentButtonState3 != lastButtonState3 && currentButtonState3 == HIGH && (currentTime - lastDebounceTime3 > debounceDelay))
  {
    ledState[2] = reverseValue(ledState[2]);
    Serial.write("bt3 click");
    lastDebounceTime3 = currentTime;
  }
  lastButtonState3 = currentButtonState3;

  // Bottone 4
  bool currentButtonState4 = digitalRead(BUT4_PIN);
  if (currentButtonState4 != lastButtonState4 && currentButtonState4 == HIGH && (currentTime - lastDebounceTime4 > debounceDelay))
  {
    ledState[3] = reverseValue(ledState[3]);
    Serial.write("bt4 click");
    lastDebounceTime4 = currentTime;
  }
  lastButtonState4 = currentButtonState4;

  delay(20);
  Serial.println("button state: " + (String)ledState[0] + " " + (String)ledState[1] + " " + (String)ledState[2] + " " + (String)ledState[3]);
  ledUpdater();
}

void ledUpdater()
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(ledPins[i], ledState[i]);
  }
}

int reverseValue(int i)
{
  return i == 0 ? 1 : 0;
}
