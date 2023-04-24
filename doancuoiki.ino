#include <ESP8266WiFi.h>
#include <DHT.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_TEMPLATE_ID "TMPL6Igh4n8uA"
#define BLYNK_TEMPLATE_NAME "Template Fire"
#define BLYNK_AUTH_TOKEN "Z1zZFR2ISftGZVnSdGxREAHKoPZlmqHo"

// Thay đổi thông tin mạng WiFi
char auth[] = "Z1zZFR2ISftGZVnSdGxREAHKoPZlmqHo";
char ssid[] = "ABC";
char pass[] = "0123456789";

// Định nghĩa chân GPIO kết nối cảm biến lửa và cảm biến khí gas
const int pinLPG = D1;    // Chân GPIO D1 cho cảm biến khí gas
const int pinFlame = D2;  // Chân GPIO D2 cho cảm biến lửa
const int pinDHT = D3;    // Chân GPIO D3 cho cảm biến nhiệt độ
const int pinBuzzer = D4; // Chân GPIO D4 cho buzzer
const int pinLed = D5;    // Chân GPIO D5 cho Led
const int pinButton = D6; // Chân GPIO D6 cho nút nhấn
DHT dht(pinDHT, DHT11);
bool buttonStatus;

void setup() {
  Serial.begin(9600);
  // Kết nối với WiFi
  WiFi.begin(ssid, pass);  // Replace with your SSID and password
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ...");
  }
  Blynk.begin(auth, ssid, pass);

  pinMode(pinLPG, INPUT);
  pinMode(pinFlame, INPUT);
  pinMode(pinButton, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLed, OUTPUT);
  dht.begin();

  Serial.println("Done! ");
  Alarm(2);
}

BLYNK_WRITE(V0) {
  buttonStatus = param.asInt();  // Đọc giá trị của nút bấm ảo từ Blynk
}

void loop() {
  Blynk.run();

  // Đọc giá trị từ cảm biến lửa và cảm biến khí gas
  int flameValue = digitalRead(pinFlame);
  int lpgValue = digitalRead(pinLPG);

  // Đọc giá trị từ cảm biến DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Đọc giá trị từ button
  buttonStatus = digitalRead(pinButton);

  // Gửi dữ liệu nhiệt độ và độ ẩm, trạng thái button lên Blynk
  if (!isnan(temperature) && !isnan(humidity)) {
    // Serial.println(temperature);
    // Serial.println(humidity);
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    Blynk.virtualWrite(V0, buttonStatus);
  } else {
    Serial.println("Không đọc được dữ liệu từ DHT11");
    digitalWrite(pinLed, HIGH);
    delay(1000);
  }

  //Kiểm tra giá trị nhiệt độ nếu trên 50 thì sẽ thông báo trên điện thoại
  if (temperature > 50) Blynk.logEvent("high_temperature");

  // Kiểm tra giá trị đọc được từ cảm biến lửa và cảm biến khí gas
  if (flameValue == 0 || lpgValue == 0 || buttonStatus) {
    // Nếu phát hiện nguy cơ lửa hoặc khí gas, bật module buzzer,led
    if (flameValue == 0) {
      Blynk.logEvent("fire");
      Serial.println("Fire!");
    }
    if (lpgValue == 0) {
      Blynk.logEvent("gas");
      Serial.println("Gas!");
    }
    if (buttonStatus) {
      Blynk.logEvent("fire");
      Serial.println("Button!");
    }
    Alarm(10);
  } else {
    // Nếu không có nguy cơ, tắt module buzzer,led
    digitalWrite(pinBuzzer, LOW);
    digitalWrite(pinLed, LOW);
  }
}

void Alarm(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(pinBuzzer, HIGH);
    // tone(pinBuzzer,30000);
    digitalWrite(pinLed, HIGH);
    delay(750);
    digitalWrite(pinBuzzer, LOW);
    // noTone(pinBuzzer);
    digitalWrite(pinLed, LOW);
    delay(250);
  }
}

