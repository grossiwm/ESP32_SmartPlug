#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <stdbool.h>
#include <String.h>

#define TOPICO_SUBSCRIBE "v1/devices/me/rpc/request/+"
#define ID_MQTT  "INCB_Cliente_MQTT"

const int relay = 26;
char charArray[100];
boolean ativado=0;
//const char* SSID = [SSID]; 
//const char* PASSWORD = [PASSWORD];

//const char* BROKER_MQTT = [BROKER IP ADDRESS]; 
//int BROKER_PORT = [BROKER PORT];

//ThingsBoard
//const char* DEV_ACCESS_TOKEN = [DEVICE ACCESS TOKEN];

WiFiClient espClient;
PubSubClient MQTT(espClient);

//prototypes
void init_wifi(void);
void init_mqtt(void);
void reconnect_wifi(void); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verifica_conexoes_wifi_mqtt(void);
void reconnect_mqtt(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  init_wifi();
  init_mqtt();
  pinMode(relay, OUTPUT);
}

void loop() {

  verifica_conexoes_wifi_mqtt();
  MQTT.loop();
}

void init_wifi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconnect_wifi();
}

void init_mqtt(void) 
{
    /* informa a qual broker e porta deve ser conectado */
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
    /* atribui função de callback (função chamada quando qualquer informação do 
    tópico subescrito chega) */
    MQTT.setCallback(mqtt_callback);            
}

void reconnect_mqtt(void) 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT, DEV_ACCESS_TOKEN, "")) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}


void reconnect_wifi() 
{

    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD);
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    msg.toCharArray(charArray, 100);
    
    StaticJsonDocument<200> doc; 
    
    DeserializationError error = deserializeJson(doc, charArray);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    return;
    } else {
      ativado = doc["params"];
      Serial.print("[MQTT] Parametro Recebido: ");
      Serial.println(ativado); 
    }
    

    digitalWrite(relay, ativado);


    delay(100);
}

void verifica_conexoes_wifi_mqtt(void)
{

    reconnect_wifi(); 

    if (!MQTT.connected()) 
        reconnect_mqtt(); 
} 
  
