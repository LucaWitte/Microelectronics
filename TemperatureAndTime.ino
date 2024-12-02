#include <WiFi.h>
#include <time.h>
#include <SevSeg.h>
#include "DHT.h"

// Pins for the 7 segment 4 digit display
int pinA = 4;int pinB = 16;int pinC = 17;int pinD = 32;
int pinE = 13;int pinF = 5;int pinG = 18;
int pinDP = 12;int D1 = 14;int D2 = 27;int D3 = 26; int D4 = 25;

// Button switching
const int buttonPin = 21;  // Pushbutton
const int ledPin = 2;    // For testing

int ledState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// To get the time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;         // Adjust for your timezone (e.g., -3600 for GMT-1)
const int daylightOffset_sec = 0; // Adjust for daylight saving time if applicable

// Variables for timing
unsigned long lastDigitUpdate = 0;
const int digitInterval = 1; // Time in ms per digit
int currentDigit = 0;

// DHT11 Temperature sensor
#define DHTPIN 15     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);

  // For button switching
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // For display
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinDP, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  
  turnOffAllDigits();
  turnOffAllSegments();

  // Connect to Wifi and get the current time from server
  WiFi.begin("NAME", "PASSWORD");
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Time synchronized with NTP server");

  Serial.println(F("Beginning Time/Temperature display!"));

  // set initial LED state
  digitalWrite(ledPin, ledState);

  dht.begin();

}

void loop() {

  char timeStr[5]; // Buffer for hhmm (4 digits + null terminator)
  int reading = digitalRead(buttonPin);

  // Button logic
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }
  // Displaying temperature
  
  if (ledState == HIGH){
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    int temp = (int)(t * 100);

    int digitValues[4] = { (temp / 1000) % 10, (temp / 100) % 10, (temp / 10) % 10, temp % 10 };

    if (t < 10.0) {
      digitValues[0] = 0; // Ensure leading zero for small temperatures
    }

    // Multiplex digits
    unsigned long currentMillis = millis();
    if (currentMillis - lastDigitUpdate >= digitInterval) {
      lastDigitUpdate = currentMillis;

      // Turn off the previous digit
      turnOffAllDigits();

      // Display the current digit
      displayDigit(digitValues[currentDigit], currentDigit + 1);

      // Move to the next digit
      currentDigit = (currentDigit + 1) % 4;
      delay(1);    
    }
  }

  if (ledState != HIGH){
    // Displaying time
    struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        // Format time as hhmm
        strftime(timeStr, sizeof(timeStr), "%H%M", &timeinfo);
        // Use the formatted time as needed
      } else {
        Serial.println("Failed to obtain time");
      }

    int time = atoi(timeStr);
    int digitValues[4] = { (time / 1000) % 10, (time / 100) % 10, (time / 10) % 10, time % 10 };
    
    unsigned long currentMillis = millis();
    if (currentMillis - lastDigitUpdate >= digitInterval) {
      lastDigitUpdate = currentMillis;

      // Turn off the previous digit
      turnOffAllDigits();

      // Display the current digit
      displayDigit(digitValues[currentDigit], currentDigit + 1);

      // Move to the next digit
      currentDigit = (currentDigit + 1) % 4;
      delay(1);    
    }
  }
  
    digitalWrite(ledPin, ledState);
   lastButtonState = reading;

}

// Function to display a single digit
void displayDigit(int num, int digitPosition) {
  // Activate the corresponding digit
  switch (digitPosition) {
    case 1:
      digit1();
      break;
    case 2:
      digit2();
      break;
    case 3:
      digit3();
      break;
    case 4:
      digit4();
      break;
    default:
      turnOffAllDigits();
      break;
  }

  // Display the number
  switch (num) {
    case 0:
      zero();
      break;
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    case 7:
      seven();
      break;
    case 8:
      eight();
      break;
    case 9:
      nine();
      break;
    default:
      turnOffAllSegments();
      break;
  }

  // Control the decimal point
  if (digitPosition == 2) {
    // Turn on the decimal point after the second digit
    digitalWrite(pinDP, LOW); // For common anode display
  } else {
    digitalWrite(pinDP, HIGH); // Turn off decimal point
  }
}

void turnOffAllDigits() {
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
}

//functions representing numbers 0-9
void zero(){
digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, HIGH);
  }

  void one(){
digitalWrite(pinA, HIGH);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, HIGH);
digitalWrite(pinF, HIGH);
digitalWrite(pinG, HIGH);
}

void two(){
  digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, HIGH);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, HIGH);
digitalWrite(pinG, LOW);
  }
  
  void three(){
    digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, LOW);
digitalWrite(pinE, HIGH);
digitalWrite(pinF, HIGH);
digitalWrite(pinG, LOW);
    }
    
  void four(){
    digitalWrite(pinA, HIGH);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, HIGH);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }
    
  void five(){
    digitalWrite(pinA, LOW);
digitalWrite(pinB, HIGH);
digitalWrite(pinC, LOW);
digitalWrite(pinD, LOW);
digitalWrite(pinE, HIGH);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }
    
  void six(){
    digitalWrite(pinA, LOW);
digitalWrite(pinB, HIGH);
digitalWrite(pinC, LOW);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }
    
  void seven(){
    digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, HIGH);
digitalWrite(pinF, HIGH);
digitalWrite(pinG, HIGH);
    }
    
  void eight(){
    digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }
    
  void nine(){
    digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, HIGH);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }

  
    void letter_L(){
  digitalWrite(pinA, HIGH);
digitalWrite(pinB, HIGH);
digitalWrite(pinC, HIGH);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, HIGH);
    }

    void letter_U(){
  digitalWrite(pinA, HIGH);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, HIGH);
   }
  
    void letter_C(){
  digitalWrite(pinA, LOW);
digitalWrite(pinB, HIGH);
digitalWrite(pinC, HIGH);
digitalWrite(pinD, LOW);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, HIGH);
    }
    
    void letter_A(){
  digitalWrite(pinA, LOW);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }

    void letter_K(){
  digitalWrite(pinA, HIGH);
digitalWrite(pinB, LOW);
digitalWrite(pinC, LOW);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }

  void letter_F(){
digitalWrite(pinA, LOW);
digitalWrite(pinB, HIGH);
digitalWrite(pinC, HIGH);
digitalWrite(pinD, HIGH);
digitalWrite(pinE, LOW);
digitalWrite(pinF, LOW);
digitalWrite(pinG, LOW);
    }

    void allNumbers(){
one();delay(500);
two();delay(500);
three();delay(500);
four();delay(500);
five();delay(500);
six();delay(500);
seven();delay(500);
eight();delay(500);
nine();delay(500);
      }

  void turnOffAllSegments(){
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
  digitalWrite(pinDP, HIGH); 
          }


    //functions to select individual digits on the display
    void digit1(){
      digitalWrite(D1, HIGH);
digitalWrite(D2, LOW);
digitalWrite(D3, LOW);
digitalWrite(D4, LOW);
      }
      
    void digit2(){
      digitalWrite(D1, LOW);
digitalWrite(D2, HIGH);
digitalWrite(D3, LOW);
digitalWrite(D4, LOW);
      }
      
    void digit3(){
      digitalWrite(D1, LOW);
digitalWrite(D2, LOW);
digitalWrite(D3, HIGH);
digitalWrite(D4, LOW);
      }
      
    void digit4(){
      digitalWrite(D1, LOW);
digitalWrite(D2, LOW);
digitalWrite(D3, LOW);
digitalWrite(D4, HIGH);
      }

      void all4Digits(){
        digitalWrite(D1, HIGH);
digitalWrite(D2, HIGH);
digitalWrite(D3, HIGH);
digitalWrite(D4, HIGH);
        }






