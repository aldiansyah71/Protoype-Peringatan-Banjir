#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Informasi WiFi
const char* ssid = "Ezerdam";
const char* password = "ezerdam4389";

// Informasi Blynk
#define BLYNK_TEMPLATE_ID "TMPL6TFu-qtUO"
#define BLYNK_TEMPLATE_NAME "BanjirIoT"
#define BLYNK_AUTH_TOKEN "9NpmmKatdKFjQ9_Unwfwq8x5pm0h2qx9"

// Pin untuk sensor raindrop
#define RAINDROP_PIN 34  

// Pin untuk sensor aliran air YF-S201
#define FLOW_SENSOR_PIN 25  
volatile int pulseCount = 0;
unsigned long oldTime = 0;
const float calibrationFactor = 4.5;

// Variabel untuk menghitung laju aliran
float flowRate;
unsigned long previousMillis = 0;
bool hujanTerkirim = false;  

// Pin untuk sensor ultrasonik HC-SR04
#define TRIG_PIN_1 12   // Pin Trigger pertama
#define ECHO_PIN_1 13   // Pin Echo pertama
#define TRIG_PIN_2 32   // Pin Trigger kedua
#define ECHO_PIN_2 33   // Pin Echo kedua
long duration, distance, duration2, distance2;

// Pin untuk kontrol relay 4-channel
#define RELAY_PIN_1 15
#define RELAY_PIN_2 26
#define RELAY_PIN_3 27
#define RELAY_PIN_4 5

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Inisialisasi Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Inisialisasi pin raindrop
  pinMode(RAINDROP_PIN, INPUT);

  // Inisialisasi sensor aliran air
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);

  // Inisialisasi pin sensor ultrasonik
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  // Inisialisasi pin relay
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);

  // Memastikan semua relay dalam keadaan mati
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);
  digitalWrite(RELAY_PIN_4, HIGH);
}

void loop() {
  Blynk.run(); // Jalankan Blynk
  
  // Pembacaan sensor raindrop
  int raindropValue = analogRead(RAINDROP_PIN);
  
  // Logika untuk mendeteksi hujan berdasarkan nilai sensor raindrop
  String rainStatus = "";
  if (raindropValue < 1700) {  // Hujan deras
    rainStatus = "Hujan deras";
  } else if (raindropValue >= 2000 && raindropValue <= 2500) {  // Hujan kecil
    rainStatus = "Hujan kecil";
  } else {
    rainStatus = "Tidak ada hujan";
  }

  // Kirim status hujan ke Blynk
  Blynk.virtualWrite(V0, rainStatus);

  // Pembacaan sensor aliran air
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    flowRate = (pulseCount / calibrationFactor);  
    pulseCount = 0;  

    Blynk.virtualWrite(V1, flowRate);
  }

  // Pembacaan sensor ultrasonik pertama
  digitalWrite(TRIG_PIN_1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_1, LOW);
  duration = pulseIn(ECHO_PIN_1, HIGH);
  distance = duration * 0.034 / 2;
  Blynk.virtualWrite(V2, distance);

  // Pembacaan sensor ultrasonik kedua
  digitalWrite(TRIG_PIN_2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_2, LOW);
  duration2 = pulseIn(ECHO_PIN_2, HIGH);
  distance2 = duration2 * 0.034 / 2;
  Blynk.virtualWrite(V3, distance2);

  delay(1000); 
}

BLYNK_WRITE(V4) {
  int pinValue = param.asInt(); 
  pinValue = !pinValue;        
  digitalWrite(RELAY_PIN_1, pinValue);
}

BLYNK_WRITE(V5) {
  int pinValue = param.asInt(); 
  pinValue = !pinValue;        
  digitalWrite(RELAY_PIN_2, pinValue);
}

BLYNK_WRITE(V6) {
  int pinValue = param.asInt(); 
  pinValue = !pinValue;        
  digitalWrite(RELAY_PIN_3, pinValue);
}

BLYNK_WRITE(V7) {
  int pinValue = param.asInt(); 
  pinValue = !pinValue;        
  digitalWrite(RELAY_PIN_4, pinValue);
}