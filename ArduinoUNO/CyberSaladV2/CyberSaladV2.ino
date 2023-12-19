/* 
  CyberSalad Kit - Monitoraggio Impianto idroponico
  Future Education Modena 2023

  Monitora: 
  - Umidità/Temperatura/Pressione: Grove Temp/Hum/Barometer (BME 680 -I2C)
  - Illuminazione: Grove Lux (analog)
  - Temperatura dell'Acqua  (DS18B20 + connettore Grove)
  - TDS: Grove TDS Sensor (analog)
  - 2 relé

  Utilizza Grove LED
  Comunica con seriale software con il WIO Terminal
*/

///// NON SPOSTARE QUESTE DEFINIZIONI  /////
//*******************************************
#define SECRET_SSID "FEM_WiFi"
#define SECRET_PASS "wifipassword"
#define CHIAVE_CLOUD "FEMCyberSalad_piante1"

char dboard_server[] = "demo.thingsboard.io"; // Indirizzo IP/Internet del Dashboard Server
int dboard_port = 80;                         // Porta IP del server

//Variabili
float temp_aria = 0;
float umid_aria = 0;
float luminosita = 0;
float temp_acqua = 0;
float livello_acqua = 0;
unsigned int ppm_tds = 0;

//Collegare una resistenza da 1K o la scheda led Grove
#define PIN_LED1 13
//Collegare RESET a questo pin con un jumper
#define PIN_RESET 12

//Collegamento alla funzione del led
void accendi_LED_per(byte volte);

//Collegamento alla piattaforma GL-Blocks
// #include "GL-Blocks-WiFi.h" - con il REV4
// #include "GL-Blocks-Dashboard-CP.h" con il REV4
//*******************************************
///// NON SPOSTARE QUESTE DEFINIZIONI  /////

#include <SoftwareSerial.h>

#include <Wire.h>  
#include <OneWire.h> 
#include <DallasTemperature.h>
// DS12B20 data plugged into pin 2 on the Arduino 
#define PIN_DS18B20 2
OneWire oneWire(PIN_DS18B20);
DallasTemperature temp_sens(&oneWire);

#include <Ultrasonic.h>
#define PIN_ULTRASUONI 3
Ultrasonic ultrasonic(PIN_ULTRASUONI);
//DISTANZA_FONDO è la distanza in centimetri tra sensore e fondo del serbatoio.
//Deve essere aggiustato rispetto all'altezza del serbaroio ed alla posizione del sensore.
#define DISTANZA_FONDO 30.0

#include "seeed_bme680.h"
//#define BME_SCK 13
//#define BME_MISO 12
//#define BME_MOSI 11
//#define BME_CS 10
#define BME680_IIC_ADDR  uint8_t(0x76)
Seeed_BME680 bme680(BME680_IIC_ADDR); /* IIC PROTOCOL */

// Pin analogici
#define PIN_ILLUMINAZ A0
#define PIN_TDS A1

// Pin canali relè
#define PIN_RELE_LAMP 4  //Canale 1
#define PIN_RELE_PUMP 5  //Canale 2

//Pin SW Serial verso WIO
#define PIN_SWSERIAL_RX 8
#define PIN_SWSERIAL_TX 7
SoftwareSerial seriale_sw =  SoftwareSerial(PIN_SWSERIAL_RX, PIN_SWSERIAL_TX);

/**
 * Preparazione di Arduino: setup() 
 * Eseguito una sola volta.
 */
void setup() {

  //Configura il pin per il reset
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  pinMode(PIN_RESET, OUTPUT);

  //Pin per la SW Serial
  pinMode(PIN_SWSERIAL_RX, INPUT);
  pinMode(PIN_SWSERIAL_TX, OUTPUT);

  //PIN ANALOGICI
  pinMode(PIN_ILLUMINAZ, INPUT);
  pinMode(PIN_TDS, INPUT);

  //Attiva il Serial Monitor
  Serial.begin(9600);  
  delay(1000); //Attesa setup della seriale
  seriale_sw.begin(9600);
  delay(1000); //Attesa setup della seriale
  Serial.println("FEM - Cyber Salad Kit V2 - Piante");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(PIN_RELE_LAMP, OUTPUT);
  digitalWrite(PIN_RELE_LAMP, LOW);
  pinMode(PIN_RELE_PUMP, OUTPUT);
  digitalWrite(PIN_RELE_PUMP, LOW);

  accendi_LED_per(1); //Lampeggia 1 volta
  
  //Wire.begin(); //Inzializza I2C per Sensore Grove

  accendi_LED_per(2); //Lampeggia 2 volte

  ///// ATTIVAZIONE DEI SENSORI /////

  //Start up del DS18B20 
  temp_sens.begin();

  //Start BME 680
  while (!bme680.init()) {
      Serial.println("bme680 init failed ! can't find device!");
      delay(10000);
  }

  accendi_LED_per(3); //Lampeggia 3 volte

  // Connessione al WiFi: vedi il file GL-Blocks-WiFi.h
  // Connetti_WIFI();  Con REV4
  
  // accendi_LED_per(4); //Lampeggia 4 volte: PRONTI   
 
}

/**
 * Arduino ciclo infinito
 */
void loop() {

  accendi_LED_per(3);

  //Temperatura ed umidità dal BME-680
  if (bme680.read_sensor_data()) Serial.println("Lettura BME680 fallita");
  else {
    temp_aria = bme680.sensor_result_value.temperature;
    umid_aria = bme680.sensor_result_value.humidity;
    //val_gas = bme680.sensor_result_value.gas / 1000.0 - Eventuale estensione
  }

  //Illuminazione dal sensore analogico Grove  
  luminosita = analogRead(PIN_ILLUMINAZ);
  luminosita = luminosita * 100 / 1024;

  //Lettura del DS18B20: temperatura acqua serbatoio 
  temp_sens.requestTemperatures();
  temp_acqua = temp_sens.getTempCByIndex(0);

  long dist_cm = ultrasonic.MeasureInCentimeters();  
  if (dist_cm > 200)
      livello_acqua = 0.0; //dist > 2m = ovviamente non valido
  else {
    livello_acqua = DISTANZA_FONDO - dist_cm; //In cm.
    if (livello_acqua < 0) livello_acqua = 0; //ignorare imprecisioni del sensore
  }

  //Lettura della sonda TDS analogica
  unsigned int val_tds = analogRead(PIN_TDS);
  ppm_tds = map(val_tds, 0, 470, 0, 100);

  accendi_LED_per(2);
      
  Mostra_Valori_Serial_Monitor();

  Gestisci_Rele();

  //Connetti_WIFI();
  //delay(1000);

  //Trasmetti_Dati_Cloud();    
  //delay(30000);
  delay(10000); //Per i test
}

/**
 * Lampeggia il LED sul pin PIN_LED1 per un numero di volte
 */
void accendi_LED_per(byte volte) 
{
  for (byte i=0; i<volte; i++) {
    digitalWrite(PIN_LED1, HIGH);
    delay(200);
    digitalWrite(PIN_LED1, LOW);
    delay(200);
  }
}

/**
 * Scrive i valori dei sensori sul serial Monitor (Serial)
 */
void Mostra_Valori_Serial_Monitor()
{
  Serial.println();    
  Serial.print("Temp. aria = ");
  Serial.print(temp_aria);
  Serial.println(" °C");

  Serial.print("Umid. aria = ");
  Serial.print(umid_aria);
  Serial.println(" %");

  Serial.print("Luminosità rel. = ");
  Serial.print(luminosita);
  Serial.println(" %");

  Serial.print("Temp. acqua = ");
  Serial.print(temp_acqua);
  Serial.println(" °C");

  Serial.print("Liv. acqua = ");
  Serial.print(livello_acqua);
  Serial.println(" cm");

  Serial.print("TDS acqua = ");
  Serial.print(ppm_tds);
  Serial.println(" ppm");  
}

/**
 * Applica la logica di attuazione dei relé
 */
void Gestisci_Rele() {

  if (luminosita < 50)
    digitalWrite(PIN_RELE_LAMP, HIGH);
  else  
    digitalWrite(PIN_RELE_LAMP, LOW);

}

