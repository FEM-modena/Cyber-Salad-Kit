/* 
  CyberSalad Kit - Monitoraggio Serbatoio acqua
  Future Education Modena 2022

  Monitora: 
  - Umidità/Temperatura (Shield MKR Env)  
  - Livello dell'Acqua idroponico (Grove Sensore Capacitivo analogico)
  - TDS/Conducibilità (Grove TDS Sensor)
  - Temperatura dell'Acqua (DS18B20 + connetore Grove)
*/

///// NON SPOSTARE QUESTE DEFINIZIONI  /////
//*******************************************
#define SECRET_SSID "FEM_WiFi"
#define SECRET_PASS "wifipassword"
#define CHIAVE_CLOUD "FEMCyberSalad_token"

char dboard_server[] = "iot.fem.digital"; // Indirizzo IP/Internet del Dashboard Server
int dboard_port = 80;                         // Porta IP del server

//Variabili
float temp_aria = 0;
float umid_aria = 0;
long livello_acqua = 0; //in cm
int ppm_tds = 0;
int temp_acqua = 0;

//Collegare una resistenza da 1K o la scheda led Grove
#define PIN_LED1 5
//Collegare RESET a questo pin con un jumper
#define PIN_RESET 4

//Collegamento alla funzione del led
void accendi_LED_per(byte volte);

//Collegamento alla piattaforma GL-Blocks
#include "GL-Blocks-WiFi.h"
#include "GL-Blocks-Dashboard-CS.h"
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

#include <Ultrasonic.h>

#define PIN_ULTRASUONI 3

Ultrasonic ultrasonic(PIN_ULTRASUONI);
//DISTANZA_FONDO è la distanza in centimetri tra sensore e fondo del serbatoio.
//Deve essere aggiustato rispetto all'altezza del serbaroio ed alla posizione del sensore.
#define DISTANZA_FONDO 30.0

#define PIN_SONDA_TDS A3

/**
 * Preparazione di Arduino: setup() 
 * Eseguito una sola volta.
 */
void setup() {

  //Configura il pin per il reset
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  pinMode(PIN_RESET, OUTPUT);

  //Attiva il Serial Monitor
  Serial.begin(9600);  
  delay(2000); //Attesa setup della seriale
  Serial.println("FEM - Cyber Salad Kit - Serbatoio");

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

  //Lettura del DS18B20: temperatura acqua serbatoio 
  temp_sens.requestTemperatures();
  temp_acqua = temp_sens.getTempCByIndex(0);

  // calcolo del tempo attraverso il pin di echo
  long dist_cm = ultrasonic.MeasureInCentimeters();  
  if (dist_cm > 200)
      livello_acqua = 0.0; //dist > 2m = ovviamente non valido
  else {
    livello_acqua = DISTANZA_FONDO - dist_cm; //In cm.
    if (livello_acqua < 0) livello_acqua = 0; //ignorare imprecisioni del sensore
  }

  //Lettura della sonda TDS analogica
  unsigned int val_tds = analogRead(PIN_SONDA_TDS);
  ppm_tds = map(val_tds, 0, 470, 0, 100);

  accendi_LED_per(2);
      
  Mostra_Valori_Serial_Monitor();

  Connetti_WIFI();
  delay(1000);

  Trasmetti_Dati_Cloud();    
  delay(30000);
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
  
  Serial.println();    
  Serial.print("Temp. acqua = ");
  Serial.print(temp_acqua);
  Serial.println(" °C");

  Serial.println();
  Serial.print("Livello serbatoio = ");
  Serial.print(livello_acqua); 
  Serial.println(" cm");

  Serial.println();
  Serial.print("Presenza sostanze acqua = ");
  Serial.print(ppm_tds); 
  Serial.println(" ppm");
    
}
