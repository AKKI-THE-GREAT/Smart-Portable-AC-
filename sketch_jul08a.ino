#include <DHT.h>
#include <LiquidCrystal.h>

// Define pin connections
#define DHTPIN1 2           // Pin for room temperature sensor
#define DHTPIN2 3           // Pin for cool air temperature sensor
#define DHTTYPE DHT22       // DHT 22 (AM2302)
#define RELAY_PIN 7         // Relay control pin for Peltier module
#define FAN_PIN 6           // Fan control pin (PWM)
#define SWITCH_PIN 8        // Switch pin to toggle automatic mode
#define TEMP_THRESHOLD 25.0 // Temperature threshold for cooling

DHT dhtRoom(DHTPIN1, DHTTYPE);
DHT dhtCool(DHTPIN2, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

bool automaticMode = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  // Initialize sensors
  dhtRoom.begin();
  dhtCool.begin();
  // Set pin modes
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Temp: ");
}

void loop() {
  // Read switch state
  automaticMode = digitalRead(SWITCH_PIN) == LOW;
  
  // Display mode on LCD
  lcd.setCursor(0, 1);
  if (automaticMode) {
    lcd.print("Auto Mode ON ");
    // Read temperatures
    float roomTemp = dhtRoom.readTemperature();
    float coolAirTemp = dhtCool.readTemperature();
    
    // Display room temperature on LCD
    lcd.setCursor(6, 0);
    lcd.print(roomTemp);
    lcd.print((char)223); // Degree symbol
    lcd.print("C ");

    // Control Peltier module and fan based on room temperature
    if (roomTemp > TEMP_THRESHOLD) {
      digitalWrite(RELAY_PIN, HIGH);
      int fanSpeed = map(roomTemp, TEMP_THRESHOLD, 40, 128, 255);
      analogWrite(FAN_PIN, fanSpeed);
    } else {
      digitalWrite(RELAY_PIN, LOW);
      analogWrite(FAN_PIN, 0);
    }

    // Print temperatures to the serial monitor for debugging
    Serial.print("Room Temp: ");
    Serial.print(roomTemp);
    Serial.print(" C, Cool Air Temp: ");
    Serial.print(coolAirTemp);
    Serial.println(" C");
  } else {
    lcd.print("Manual Mode  ");
    digitalWrite(RELAY_PIN, LOW);
    analogWrite(FAN_PIN, 0);
  }

  // Wait for a second before the next loop
  delay(1000);
}
