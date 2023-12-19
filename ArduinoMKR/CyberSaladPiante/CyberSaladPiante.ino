/* 
  CyberSalad Kit - Monitoraggio Piante idroponico
  Future Education Modena 2022

  Monitora: 
  - Umidità/Temperatura (Shield MKR Env)  
  - Illuminazione (Shield MKR Env)
  - Indice UV (Shield MKR Env)
  - Umidità del terreno (Sensori Umidità capacitivi Grove)
  - Temperatura dell'Acqua  (DS18B20 + connetore Grove)
*/

///// NON SPOSTARE QUESTE DEFINIZIONI  /////
//*******************************************
#define SECRET_SSID "FEM_WiFi"
#define SECRET_PASS "wifipassword"
#define CHIAVE_CLOUD "FEMCyberSalad_piante1"

char dboard_server[] = "iot.fem.digital"; // Indirizzo IP/Internet del Dashboard Server
int dboard_port = 80;                         // Porta IP del server

//Variabili
float temp_aria = 0;
float umid_aria = 0;
float luminosita = 0;
int umid_terreno1 = 0;
int umid_terreno2 = 0;
int umid_terreno3 = 0;
float indiceUV = 0;
float temp_acqua = 0;

//Collegare una resistenza da 1K o la scheda led Grove
#define PIN_LED1 5
//Collegare RESET a questo pin con un jumper
#define PIN_RESET 4

//Collegamento alla funzione del led
void accendi_LED_per(byte volte);

//Collegamento alla piattaforma GL-Blocks
#include "GL-Blocks-WiFi.h"
#include "GL-Blocks-Dashboard-CP.h"
//*******************************************
///// NON SPOSTARE QUESTE DEFINIZIONI  /////

#include <Wire.h>  

//Libreria ENV Shield di Arduino
//Installare da "Gestione Librerie"
#include <Arduino_MKRENV.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

// DS12B20 data plugged into pin 2 on the Arduino 
#define PIN_DS18B20 2
OneWire oneWire(PIN_DS18B20);
DallasTemperature temp_sens(&oneWire);

// Pin analogici di umidità terreno
#define PIN_UMIDITA_1 A1
#define PIN_UMIDITA_2 A3
#define PIN_UMIDITA_3 A4

/**
 * Preparazione di Arduino: setup() 
 * Eseguito una sola volta.
 */
void setup() {

  //Configura il pin per il reset
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  pinMode(PIN_RESET, OUTPUT);

  pinMode(PIN_UMIDITA_1, INPUT);
  pinMode(PIN_UMIDITA_2, INPUT);
  pinMode(PIN_UMIDITA_3, INPUT);

  //Attiva il Serial Monitor
  Serial.begin(9600);  
  delay(2000); //Attesa setup della seriale
  Serial.println("FEM - Cyber Salad Kit - Piante");

  pinMode(PIN_LED1, OUTPUT);
  digitalWrite(PIN_LED1, LOW);

  accendi_LED_per(1); //Lampeggia 1 volta
  
  Wire.begin(); //Inzializza I2C per l'ENV shield 
  accendi_LED_per(2); //Lampeggia 2 volte

  ///// ATTIVAZIONE DEI SENSORI /////
 
  //Si connette L'ENV Shield
  if (!ENV.begin()) {
    Serial.println("Errore durante l'avvio del MKR Shield");
    while (1);
  }   

  //Start up del DS18B20 
  temp_sens.begin();

  accendi_LED_per(3); //Lampeggia 3 volte

  // Connessione al WiFi: vedi il file GL-Blocks-WiFi.h
  Connetti_WIFI();  
  
  accendi_LED_per(4); //Lampeggia 4 volte: PRONTI   
 
}

/**
 * Arduino ciclo infinito
 */
void loop() {

  accendi_LED_per(3);

  //Temperatura dall'ENV Shield
  temp_aria = ENV.readTemperature();

  //Umidità aria dall'ENV Shield
  umid_aria = ENV.readHumidity();

  //Illuminazione dall'ENV Shield
  luminosita = ENV.readIlluminance();

  //IndiceUV dall'ENV Shield
  indiceUV = ENV.readUVIndex();

  //Lettura del DS18B20: temperatura acqua serbatoio 
  temp_sens.requestTemperatures();
  temp_acqua = temp_sens.getTempCByIndex(0);

  //Lettura dei tre sensori di umidità terreno
  umid_terreno1 = leggi_sens_umidita(PIN_UMIDITA_1);
  umid_terreno2 = leggi_sens_umidita(PIN_UMIDITA_2);
  umid_terreno3 = leggi_sens_umidita(PIN_UMIDITA_3);

  accendi_LED_per(2);
      
  Mostra_Valori_Serial_Monitor();

  Connetti_WIFI();
  delay(1000);

  Trasmetti_Dati_Cloud();    
  delay(30000);
}

/**
 * Procedura di lettura umidità terreno
 */
int leggi_sens_umidita(int pin)
{
  delay(10); //Breve ritardo per letture analogiche consecutive  
  long va = analogRead(pin);
  int umid = (int)map(va, 520, 760, 100, 0); //Tarato sui sensori capacitivi Grove
  umid = constrain(umid, 0, 100);
  return umid;
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

  Serial.print("Luminosità = ");
  Serial.print(luminosita);
  Serial.println(" lux");

  Serial.print("Indice UV = ");
  Serial.print(indiceUV);
  Serial.println();

  Serial.print("Umid. terr. s.1 = ");
  Serial.print(umid_terreno1);
  Serial.println(" %");

  Serial.print("Umid. terr. s.2 = ");
  Serial.print(umid_terreno2);
  Serial.println(" %");

  Serial.print("Umid. terr. s.3 = ");
  Serial.print(umid_terreno3);
  Serial.println(" %");

  Serial.print("Temp. acqua = ");
  Serial.print(temp_acqua);
  Serial.println(" °C");
}
