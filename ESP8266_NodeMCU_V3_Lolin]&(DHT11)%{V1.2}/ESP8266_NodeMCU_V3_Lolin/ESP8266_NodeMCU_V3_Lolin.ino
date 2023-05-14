/* 
[ESP8266_NodeMCU_V3_Lolin]&(DHT11)%{V1.2}

  Microcontrolador: ESP8266
  Placa: NodeMCU V3 Lolin
  Sensor: DHT11
  Versão firmware: 1.2
    - Com RTC
    - Sem SD Card

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <RTClib.h>
#include <StringSplitter.h>

WiFiClient espClient;
PubSubClient client(espClient);
RTC_DS1307 rtc;
DHTesp dht;


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
  int delay_callback;
  int delay_message;
};

struct mqtt_publish{
  char* device_init_status;
  char* sensor1;
  char* sensor2;
  char* sensor3;
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
  helper = (global_helper) {1, 0, "", 2000, 1};  //DispatchInterval, LastUpload, JSON, Delay_Callback (Milisegundos), Delay_Message (Minutos)
}

void initSubTopics(){
  subs = (mqtt_subscriber) {"Home/ESP8266/L1"};
}

void initPubTopics(){
  pub = (mqtt_publish) {"Home/ESP8266/JSON/init/SUCESS", "Home/ESP8266/simple/umidade/DHT11", "Home/ESP8266/simple/temperatura/DHT11", "Home/ESP8266/simple/UmidSolo/pressostato", "Home/ESP8266/JSON/umidade/DHT11", "Home/ESP8266/JSON/temperatura/DHT11"};
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  //armazena msg recebida em uma string
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

  delay(helper.delay_callback);
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
      //subscreve no tópico com nivel de qualidade QoS 0
      client.subscribe(subs.actuator1, 0);
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

void initRTC(){
  if (! rtc.begin()) {
    Serial.println("RTC não encontrado");
    initRTC();
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // Quando for necessário definir a hora em um novo dispositivo ou após uma queda de energia, 
    // a linha a seguir define o RTC para a data e hora em que este sketch foi compilado
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Esta linha define o RTC com uma data e hora explícitas, por exemplo, para definir 1 de Janeiro de 2023 às 3am:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
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
  initRTC();
  deviceInitStatus();
  client.subscribe(subs.actuator1, 0);
}

void setup() {
  Serial.begin(9600);
  initConfig();
  dht.setup(14, DHTesp::DHT11); // Conecta o sensor DHT ao GPIO 5

}

void deviceInitStatus(){
  String json_str = convertToJson(pub.device_init_status, 0);
  Serial.println("Inicialização do dispositivo: ");
  Serial.println(helper.json_converted);
  client.publish(pub.device_init_status, String(json_str).c_str(), true);
  helper.json_converted = "";
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
  DateTime now = rtc.now();
  StringSplitter *topic_split = new StringSplitter(topic, '/', 5); 
  // A Quantidade máxima de splits é 5 por padrão e pode ser substituído em StringSplitter.h

  helper.json_converted.concat("{ 'local' : '");
  helper.json_converted.concat(topic_split->getItemAtIndex(0));
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'device_name' : '"); 
  helper.json_converted.concat(topic_split->getItemAtIndex(1));
  helper.json_converted.concat("', ");
  if(topic_split->getItemAtIndex(4) == "SUCESS"){
    helper.json_converted.concat(" 'status' : '"); 
    helper.json_converted.concat(topic_split->getItemAtIndex(4));    
  }else{
    helper.json_converted.concat(" 'sensor_name' : '"); 
    helper.json_converted.concat(topic_split->getItemAtIndex(4));
  }
  helper.json_converted.concat("', ");
  if(topic_split->getItemAtIndex(3) == "init"){
    helper.json_converted.concat(" 'log' : '");
    helper.json_converted.concat(topic_split->getItemAtIndex(3));  
  }else{
    helper.json_converted.concat(" 'type' : '"); 
    helper.json_converted.concat(topic_split->getItemAtIndex(3));
  }
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'data' : '"); 
  helper.json_converted.concat(String(data));
  helper.json_converted.concat("', ");
  helper.json_converted.concat(" 'date_time' : '");
  helper.json_converted.concat(now.day());
  helper.json_converted.concat("/");  
  helper.json_converted.concat(now.month());
  helper.json_converted.concat("/");  
  helper.json_converted.concat(now.year());
  helper.json_converted.concat(" ");
  helper.json_converted.concat(now.hour());
  helper.json_converted.concat(":");
  helper.json_converted.concat(now.minute());
  helper.json_converted.concat(":");
  helper.json_converted.concat(now.second());  
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


  DateTime now = rtc.now();
//  Serial.println("now.minute()");
//  Serial.println(now.minute());
  if ((now.minute() - helper.lastUpload) > helper.dispatchInterval){
    sendMQTT();
    helper.lastUpload = now.minute();
  }

//  Serial.println("millis(): ");
//  Serial.println(millis());
//  if ((millis() - helper.lastUpload) > helper.dispatchInterval){
//    sendMQTT();
//    helper.lastUpload = millis();
//  }


  client.loop();  
}