#include <Firebase_ESP_Client.h>
#include <WiFi.h>
#include <PZEM004Tv30.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
const char* ssid = "PTCL FF ECCL";
const char* password = "ecc9212786";

// Insert Firebase project API Key
#define API_KEY "AIzaSyBdBDXyjPZMg1QEPDqb9t3xAkZvHcx5Zh8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://homeenergymeter-490cf-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long readDataPrevMillis = 0;
// AC Parameters
float voltage = 0.0, current = 0.0, amps1 = 0.0, amps2 = 0.0, amps3 = 0.0, pow1 = 0.0, pow2 = 0.0, pow3 = 0.0;
float power = 0.0, pf, freq = 0.0, units = 0.0, units1 = 0.0, units2 = 0.0, units3 = 0.0;
bool signupOK = false;

String resetSW = "0";

#define RXD2 16  // to TX of Pzem module
#define TXD2 17  // to RX of Pzem module

// PZEM004Tv30 pzem(Serial2, RXD2, TXD2);
PZEM004Tv30 pzem1(Serial2, RXD2, TXD2, 0x10);
PZEM004Tv30 pzem2(Serial2, RXD2, TXD2, 0x07);
PZEM004Tv30 pzem3(Serial2, RXD2, TXD2, 0x01);

void setup() {
  Serial.begin(115200);
  initWiFi();
  initFirebase();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ECC Controls");
  lcd.setCursor(0, 1);
  lcd.print("And Automation");
  lcd.setCursor(0, 2);
  // if (!pzem.setAddress(0x07)) {
  //   // Setting custom address failed. Probably no PZEM connected
  //   Serial.println("Error setting address.");
  // } else {
  //   // Print out the new custom address
  //   Serial.print("Current address:    0x");
  //   Serial.println(pzem.readAddress(), HEX);
  //   Serial.println();
  // }

  lcd.print(pzem1.readAddress());
  lcd.print("  ");
  lcd.print(pzem2.readAddress());
  lcd.print("  ");
  lcd.print(pzem3.readAddress());
  lcd.print("  ");
  delay(1000);
}

unsigned long lastMillis = 0;
bool swapScreen = false;
void loop() {
  if (millis() - lastMillis >= 4000) {
    // if WiFi is down, try reconnecting
    if ((WiFi.status() != WL_CONNECTED)) {
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
    read_data();
    scanPzemModule();
    lcdUpdate();

    lastMillis = millis();
  }
}

void lcdUpdate() {
  if (swapScreen) {
    lcdUpdate2();
    swapScreen = false;
  } else {
    lcdUpdate1();
    swapScreen = true;
  }
}

void lcdUpdate2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voltage = ");
  lcd.print(voltage, 1);
  lcd.print(" V");
  lcd.setCursor(0, 1);
  lcd.print("Current = ");
  lcd.print(current, 2);
  lcd.print(" A");
  lcd.setCursor(0, 2);
  lcd.print("Power = ");
  lcd.print(power, 2);
  lcd.print(" kW");
  lcd.setCursor(0, 3);
  lcd.print("Units = ");
  lcd.print(units, 2);
  lcd.print(" KWh");
}

void lcdUpdate1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("I= ");
  lcd.print(amps1, 2);
  lcd.print("A  ");
  lcd.print("P=");
  lcd.print(pow1, 1);
  lcd.print("W");

  lcd.setCursor(0, 1);
  lcd.print("I= ");
  lcd.print(amps2, 2);
  lcd.print("A  ");
  lcd.print("P=");
  lcd.print(pow2, 1);
  lcd.print("W");

  lcd.setCursor(0, 2);
  lcd.print("I= ");
  lcd.print(amps3, 2);
  lcd.print("A  ");
  lcd.print("P=");
  lcd.print(pow3, 1);
  lcd.print("W");

  lcd.setCursor(0, 3);
  lcd.print("PF= ");
  lcd.print(pf, 2);
  // lcd.print("   Freq= ");
  // lcd.print(freq, 1);
  // lcd.print(" Hz");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void initFirebase() {
  Serial.printf("Firebase Client v % s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf(" % s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Success");
  // connectionEstablished();
}

void read_data() {
  if (Firebase.ready() && signupOK && (millis() - readDataPrevMillis > 4000)) {
    if (Firebase.RTDB.getString(&fbdo, "/Button/resetUnits")) {
      if (fbdo.dataType() == "string") {
        resetSW = fbdo.stringData();
        if (resetSW == "1") {
          pzem1.resetEnergy();
          pzem2.resetEnergy();
          pzem3.resetEnergy();
          delay(1000);
          Firebase.RTDB.setString(&fbdo, "/Button/resetUnits", "0");
        }
      }
    }
    readDataPrevMillis = millis();
  }
}

void scanPzemModule() {
  // Read the data from the sensor
  float voltage1 = pzem1.voltage();
  amps1 = pzem1.current();
  pow1 = pzem1.power();
  units1 = pzem1.energy();
  float freq1 = pzem1.frequency();
  float pf1 = pzem1.pf();

  float voltage2 = pzem2.voltage();
  amps2 = pzem2.current();
  pow2 = pzem2.power();
  units2 = pzem2.energy();
  float freq2 = pzem2.frequency();
  float pf2 = pzem2.pf();

  float voltage3 = pzem3.voltage();
  amps3 = pzem3.current();
  pow3 = pzem3.power();
  units3 = pzem3.energy();
  float freq3 = pzem3.frequency();
  float pf3 = pzem3.pf();

  power = (pow1 + pow2 + pow3) / 1000.0;
  current = amps1 + amps2 + amps3;
  freq = (freq1 + freq2 + freq3) / 3.0;
  pf = pf1;
  units = units1 + units2 + units3;

  // Check if the data is valid
  if (isnan(voltage1)) {
    voltage1 = 0;
  }
  if (isnan(voltage2)) {
    voltage2 = 0;
  }
  if (isnan(voltage3)) {
    voltage3 = 0;
  }
  voltage = (voltage1 + voltage2 + voltage3) / 3.0;

  // Send all values to Firebase Cloud every 10sec
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000)) {
    // Serial.println("Data Sent");
    Firebase.RTDB.setString(&fbdo, "/Data/voltage", String(voltage, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/voltage1", String(voltage1, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/voltage2", String(voltage2, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/voltage3", String(voltage3, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/current", String(current, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/amps1", String(amps1, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/amps2", String(amps2, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/amps3", String(amps3, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/pow1", String(pow1, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/pow2", String(pow2, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/pow3", String(pow3, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/power", String(power, 1));
    Firebase.RTDB.setString(&fbdo, "/Data/units1", String(units1, 3));
    Firebase.RTDB.setString(&fbdo, "/Data/units2", String(units2, 3));
    Firebase.RTDB.setString(&fbdo, "/Data/units3", String(units3, 3));
    Firebase.RTDB.setString(&fbdo, "/Data/units", String(units, 2));
    Firebase.RTDB.setString(&fbdo, "/Data/freq", String(freq, 2));
    Firebase.RTDB.setString(&fbdo, "/Data/pf", String(pf, 2));
    sendDataPrevMillis = millis();
  }
}
