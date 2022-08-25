/****
 * GL Blocks
 * Piattaforma per la programmazione a blocchi
 * Green Lab - Future Education Modena 2020-2021
 */
 
unsigned long lastConnectionTime = 0;   // tempo dell'ultima connessione al server
#define POSTING_INTERVAL 30000L         // Intervallo tra gli aggiornamenti dashboard in milliseconds

//Libreria JSON
#include <ArduinoJson.h>  
//vedi https://arduinojson.org/v5/assistant/
const size_t json_capacity = JSON_OBJECT_SIZE(8) + 260; 
StaticJsonDocument<json_capacity> doc;

/*************************************
 Funzioni dei BLOCCHI
 *************************************/
/**
 * Invia i dati con interfaccia HTTP
 */
void Trasmetti_Dati_Cloud() 
{

/**
 * https://thingsboard.io/docs/reference/http-api/
 * 
 * POST http://localhost:8080/api/v1/$ACCESS_TOKEN/telemetry --header "Content-Type:application/json"
 * 
 * JSON:
{
  "temp_aria": <temp>,
  "umid_aria": <hum>,
  "luminosita": <val_lux>,
  "indiceUV": <val_UVidx>  
  "umid_terreno1": <hum_terr_1>,
  "umid_terreno2": <hum_terr_2>,
  "umid_terreno3": <hum_terr_3>,
  "temp_acqua": <temp_acqua>
}
 */

  // Carica le misure nel documento JSON
  doc["temp_aria"] = temp_aria;
  doc["umid_aria"] = umid_aria;
  doc["luminosita"] = luminosita;
  doc["indiceUV"] = indiceUV;
  doc["umid_terreno1"] = umid_terreno1;
  doc["umid_terreno2"] = umid_terreno2;
  doc["umid_terreno3"] = umid_terreno3;
  doc["temp_acqua"] = temp_acqua;

  // Close any connection before send a new request.
  // This will free the socket on the Nina module
  client.stop();

  Serial.println("\nConnessione al server IoT");
  if (client.connect(dboard_server, dboard_port)) 
  {
    Serial.println("Connesso al Cloud IoT");
    // Make a HTTP request:
    client.println("POST /api/v1/" CHIAVE_CLOUD "/telemetry HTTP/1.1"); 
    client.println("Host: ");  
    client.println("Connection: close");  
    client.print("Content-Length: ");  
    client.println(measureJson(doc));  
    client.println("Content-Type: application/json");  
    // Terminate headers with a blank line
    client.println();
    // Send JSON document in body
    serializeJson(doc, client);

    // note the time that the connection was made:
    lastConnectionTime = millis();
    
    Serial.println("Misure inviate al Cloud IoT");
    accendi_LED_per(3);
    
  } else {
    
    // if you couldn't make a connection:    
    Serial.println("Connessione al Cloud IoT non riuscita.");
    accendi_LED_per(4);
  }
  
}
