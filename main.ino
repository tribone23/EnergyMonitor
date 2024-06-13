

  // Data Communication Final Project

  // Author : execute1d

  // Internet Engineering Technology Tribone 2023

  // Politeknik Elektronika Negeri Surabaya © 2024


#define BLYNK_TEMPLATE_ID " Your Template ID "
#define BLYNK_TEMPLATE_NAME " Yout Template Name"
#define BLYNK_AUTH_TOKEN " Your Auth Token "

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>

  // Replace with your Blynk Auth Token
char auth[] = BLYNK_AUTH_TOKEN;

PZEM004Tv30 pzem(13, 12); // D7 TX, D6 RX

LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA D2, SCL D1

BlynkTimer timer;

bool wifiConnected = false;

byte fullBar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void showLoadingAnimation() {
  lcd.clear();
  lcd.setCursor(0, 0);
  int p = 1;
  lcd.print("Initializing...");
  for (int j = 0; j <= 7;++j){
    for (int i = 0; i <= 16; ++i) { // Adjust the loop count as needed
      lcd.setCursor(i, 1);
      lcd.write(byte(0));
      delay(120); // Adjust the delay as needed
      lcd.setCursor(i, 1);
      lcd.print(" ");
   }
   if (p == 0){
   lcd.setCursor(0, 0);
   lcd.print("Initializing...");
   p = 1;
   } else{
   lcd.setCursor(0, 0);
   lcd.print("Starting Up....");
   p = 0;
   }
   
  }
  lcd.setCursor(0, 0);
  lcd.print("Waiting Response");  
  lcd.setCursor(0, 1);
  lcd.print("Check Wifi AP !");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, fullBar);

  // Start Loading Animation first, then WiFi check procedure

  showLoadingAnimation();
  // Initialize WiFi Manager  
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180); 

  // Uncomment line below to reset WiFi credentials on startup.
  //wifiManager.resetSettings();

  // if WiFi is outage, it will start access point web portal, if it reach timeout, it will proceed.
  
  if (!wifiManager.autoConnect("IoT Energy Monitor v.1.2","tribone23")) {
    Serial.println("Failed to connect to WiFi and hit timeout");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Timeout/Canceled");
    lcd.setCursor(0, 1);
    lcd.print("Startup Offline!");
    wifiConnected = false;
    delay(5000);
  } else {
    // If you get here, you have connected to the WiFi
    Serial.println("Connected to WiFi");
    wifiConnected = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("Startup Online !");
    delay(5000);

    Blynk.config(auth);
    Blynk.connect();
  }

  // Assign timer to read sensor data and send it to Blynk
  timer.setInterval(1000L, sendData);
  lcd.clear();
}

void sendData() {
  int voltage = pzem.voltage();
  int current = pzem.current();
  int power = pzem.power();
  int energy = pzem.energy();

  if (!isnan(voltage) && voltage <= 260 ) {
    if (wifiConnected) Blynk.virtualWrite(V1, voltage);
    lcd.setCursor(0, 0);
    if(voltage <=9)
    {lcd.print("V: " + String(voltage) + "    ");}
    if(voltage >9 && voltage <=99)
    {lcd.print("V: " + String(voltage) + "   ");}
    if(voltage >99)
    {lcd.print("V: " + String(voltage) + "  ");}
  } else {
    lcd.setCursor(0, 0);
    lcd.print("V: N/A  ");
    if (wifiConnected) Blynk.virtualWrite(V1, 0);
  }

  // Update current
  if (!isnan(current) && current <= 100) {
    if (wifiConnected) Blynk.virtualWrite(V2, current);
    lcd.setCursor(0, 1);
    if(current <=9)
    {lcd.print("A: " + String(current) + "    ");}
    if(current >9 && current <=99)
    {lcd.print("A: " + String(current) + "   ");}
    if(current >99)
    {lcd.print("A: " + String(current) + "  ");}
  } else {
    lcd.setCursor(0, 1);
    lcd.print("A: N/A  ");
    if (wifiConnected) Blynk.virtualWrite(V2, 0);
  }

  // Update power
  if (!isnan(power)&& power <= 26000) {
    if (wifiConnected) Blynk.virtualWrite(V3, power);
    lcd.setCursor(8, 0);
    lcd.print("W: " + String(power) + "    ");
  } else {
    lcd.setCursor(8, 0);
    lcd.print("W: N/A  ");
    if (wifiConnected) Blynk.virtualWrite(V3, 0);
  }

  // Update energy
  if (!isnan(energy) && energy <= 9999) {
    if (wifiConnected) Blynk.virtualWrite(V4, energy);
    lcd.setCursor(8, 1);
    lcd.print("E: " + String(energy) + "   ");
  } else {
    lcd.setCursor(8, 1);
    lcd.print("E: N/A  ");
  }
  yield();
}

void loop() {
  if (wifiConnected && WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
  timer.run();
  yield();
}
