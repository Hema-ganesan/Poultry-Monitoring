#include <SimpleDHT.h>
#include <HardwareSerial.h>

#define DHT_PIN     4
#define MQ135_PIN   34
#define FAN_PIN     25
#define PUMP_PIN    26
#define RX_PIN      16
#define TX_PIN      17

SimpleDHT11 dht(DHT_PIN);
HardwareSerial sim800l(1);

unsigned long lastCheckTime = 0;
const unsigned long interval = 30000;

void setup() {
  Serial.begin(115200);
  sim800l.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(PUMP_PIN, LOW);

  Serial.println("System initialized...");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastCheckTime >= interval) {
    lastCheckTime = currentMillis;

    int nh3Value = analogRead(MQ135_PIN);
    byte temperature = 0;
    byte humidity = 0;
    int err = dht.read(&temperature, &humidity, NULL);

    Serial.println("\n--- Monitoring Data ---");
    Serial.print("NH3 Level: ");
    Serial.println(nh3Value);

    // NH3 condition
    if (nh3Value > 1056) {
      Serial.println("High NH3 detected. Calling user...");
      callUser();
    }

    if (err == SimpleDHTErrSuccess) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      // Fan logic
      if (temperature > 30) {
        digitalWrite(FAN_PIN, HIGH);
        Serial.println("Fan: ON");
      } else {
        digitalWrite(FAN_PIN, LOW);
        Serial.println("Fan: OFF");
      }

      // Pump logic
      if (temperature > 36) {
        digitalWrite(PUMP_PIN, HIGH);
        Serial.println("Pump: ON");
      } else {
        digitalWrite(PUMP_PIN, LOW);
        Serial.println("Pump: OFF");
      }

    } else {
      Serial.println("DHT read failed!");
    }

    Serial.println("------------------------");
  }
}

void callUser() {
  sim800l.println("ATD+917200015684;"); // Replace with your number
  delay(10000);  // Ring for 10 seconds
  sim800l.println("ATH");
  Serial.println("Call ended.");
}