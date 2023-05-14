/* 
[ESP8266_NodeMCU_V3_Lolin]&(DHT11)%{V1.1}

  Microcontrolador: ESP8266
  Placa: NodeMCU V3 Lolin
  Sensor: DHT11
  Versão firmware: 1.1
    - Sem RTC
    - Sem SD Card

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <StringSplitter.h>

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;
int tempo = 2000;


struct wifi{
  char* ssid;
  char* password;
};

struct broker_mqtt{
  char* server;
  char* user;
  char* password;
  uint16_t port;
  char* clientID;
};

struct global_helper{
  int dispatchInterval;
  int lastUpload;
  String json_converted;
};

struct mqtt_publish{
  char* sensor1;
  char* sensor2;
  char* sensor1_json;
  char* sensor2_json;
};

struct mqtt_subscriber{
  const char* actuator1;
};

struct wifi wifi_information;
struct broker_mqtt broker;
struct global_helper helper;
struct mqtt_publish pub;
struct mqtt_subscriber subs;

void initWifiInfo(){
  wifi_information = (wifi) {"<Str: Wifi SSID>", "<Str: Senha>"}; //SSID, Senha
}

void initServerBrokerInfo(){
  broker = (broker_mqtt) {"<Str: Servidor Broker>", "<Str: Usuario>", "<Str: Senha>", "<int: Porta>", "<Str: ClientID>"}; //Sevidor, User, Passwd, Port, ClientID
}


void initGlobalHelperInfo(){
  helper = (global_helper) {2000, 0, ""};  //dispatchInterval, lastUpload, JSON
}

void initSubTopics(){
  subs = (mqtt_subscriber) {"Home/ESP8266/L1"};
}

void initPubTopics(){
  pub = (mqtt_publish) {"Home/ESP8266/simple/umidade/DHT11", "Home/ESP8266/simple/temperatura/DHT11", "Home/ESP8266/JSON/umidade/DHT11", "Home/ESP8266/JSON/temperatura/DHT11"};
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);
  String strTopic = topic;
  String subsActuator1(subs.actuator1);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");

  if(strTopic == subsActuator1){
    if(strMSG == "1"){
      digitalWrite(LED_BUILTIN, LOW);
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  delay(tempo);
}


void initWifiConn(){

  WiFi.begin(wifi_information.ssid, wifi_information.password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi..");
  }
  Serial.println("Conectado na rede WiFi");

  client.setServer(broker.server, broker.port);
  client.setCallback(callback);

}

void reconectBroker() {
  //Enquanto estiver desconectado no wifi
  while (!client.connected()) {
    Serial.print("Tentando conectar ao servidor MQTT");
     
    bool conectado = strlen(broker.user) > 0 ?
                     client.connect(broker.clientID, broker.user, broker.password) :
                     client.connect(broker.clientID);
 
    if(conectado) {
      Serial.println("Conectado!");
      //subscreve no tópico com nivel de qualidade QoS 1
      client.subscribe(subs.actuator1, 1); 

    } else {
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}

void initBrokerConn(){
  //Enquanto estiver conectado ao wifi
  while (!client.connected()) {
    Serial.println("Conectando ao Broker MQTT...");
 
    if (client.connect(broker.clientID, broker.user, broker.password )) {
      Serial.println("Conectado");
    } else {
      Serial.print("falha estado  ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}


void initConfig(){
  initGlobalHelperInfo();
  initWifiInfo();
  initServerBrokerInfo();
  initWifiConn();
  initBrokerConn(); 
  initPubTopics();
  initSubTopics();
  client.subscribe(subs.actuator1);
}

void setup() {
  Serial.begin(9600);
  initConfig();
  dht.setup(14, DHTesp::DHT11); // Conectar sensor DHT no GPIO 5

}

void sendHumid(){
  delay(dht.getMinimumSamplingPeriod());
  float humidade = dht.getHumidity();
  Serial.println("Humidade: ");
  Serial.println(humidade);    
  client.publish(pub.sensor1, String(humidade).c_str(), true);
}

void sendTemp(){
  delay(dht.getMinimumSamplingPeriod());
  float temperatura = dht.getTemperature();
  Serial.println("Temperatura: ");
  Serial.println(temperatura);  
  client.publish(pub.sensor2, String(temperatura).c_str(), true);
}


void sendHumidJSON(){
  delay(dht.getMinimumSamplingPeriod());
  float umidade = dht.getHumidity();
  String json_str = convertToJson(pub.sensor1_json, umidade);
  Serial.println("HumidJSON: ");
  Serial.println(helper.json_converted);
  client.publish(pub.sensor1_json, String(json_str).c_str(), true);
  helper.json_converted = "";
}

void sendTempJSON(){
  delay(dht.getMinimumSamplingPeriod());
  float temperatura = dht.getTemperature();
  String json_str = convertToJson(pub.sensor2_json, temperatura);
  Serial.println("TempJSON: ");
  Serial.println(helper.json_converted);
  client.publish(pub.sensor2_json, String(json_str).c_str(), true);
  helper.json_converted = "";
}

String convertToJson(char* topic, float data){
  StringSplitter *topic_split = new StringSplitter(topic, '/', 5); 
  // A Quantidade máxima de splits é 5 por padrão e pode ser substituído em StringSplitter.h

  helper.json_converted.concat("{ 'local' : '");
  helper.json_converted.concat(topic_split->getItemAtIndex(0));
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'device_name' : '"); 
  helper.json_converted.concat(topic_split->getItemAtIndex(1));
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'sensor_name' : '"); 
  helper.json_converted.concat(topic_split->getItemAtIndex(4));
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'type' : '"); 
  helper.json_converted.concat(topic_split->getItemAtIndex(3));
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'data' : '"); 
  helper.json_converted.concat(String(data));  
  helper.json_converted.concat("' }");
  return String(helper.json_converted).c_str();
  
}

void sendMQTT(){
  sendHumid();
  sendTemp();
  sendHumidJSON();
  sendTempJSON();
}

void loop() {
  //Reconecta ao Wifi  
  if (WiFi.status() != WL_CONNECTED){
    initWifiConn();
  }
  //Reconecta ao Broker
  if (!client.connected()) {
    reconectBroker();
  }

  if ((millis() - helper.lastUpload) > helper.dispatchInterval){
    sendMQTT();
    helper.lastUpload = millis();
  }


  client.loop();  
}