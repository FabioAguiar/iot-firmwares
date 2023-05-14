/* 
[ESP8266_NodeMCU_V3_Lolin]&(4-RELAY-MODULE)%{V1.1}

  Microcontrolador: ESP8266
  Placa: NodeMCU V3 Lolin
  Sensor: Sem sensor
  Versão firmware: 1.1
    - Sem RTC
    - Sem SD Card

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);


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
  int pin_D5;
  int pin_D6;
  int pin_D7;
  int pin_D8;
};

struct mqtt_subscriber{
  const char* led_board;
  const char* actuator1;
  const char* actuator2;
  const char* actuator3;
  const char* actuator4;
};

struct wifi wifi_information;
struct broker_mqtt broker;
struct global_helper helper;
struct mqtt_subscriber subs;

void initWifiInfo(){
  wifi_information = (wifi) {"<Str: Wifi SSID>", "<Str: Senha>"}; //SSID, Senha
}

void initServerBrokerInfo(){
  broker = (broker_mqtt) {"<Str: Servidor Broker>", "<Str: Usuario>", "<Str: Senha>", "<int: Porta>", "<Str: ClientID>"}; //Sevidor, User, Passwd, Port, ClientID
}


void initGlobalHelperInfo(){
  helper = (global_helper) {1, 0, "", 2000, 14, 12, 13, 15};  //DispatchInterval, LastUpload, JSON, Delay_Callback (Milisegundos), Pino D5, Pino D6, Pino D7, Pino D8
}

void initSubTopics(){
  subs = (mqtt_subscriber) {"Home/ESP8266/L1", "Home/ESP8266/simple/manual/actuator01", "Home/ESP8266/simple/manual/actuator02", "Home/ESP8266/simple/manual/actuator03", "Home/ESP8266/simple/manual/actuator04"};
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);
  String strTopic = topic;
  String subsLedBoard(subs.led_board);
  String subsActuator1(subs.actuator1);
  String subsActuator2(subs.actuator2);
  String subsActuator3(subs.actuator3);
  String subsActuator4(subs.actuator4);      
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(helper.pin_D5, OUTPUT); // Conecta o atuador ao GPIO 14, Pino D5
  pinMode(helper.pin_D6, OUTPUT); // Conecta o atuador ao GPIO 12, Pino D6
  pinMode(helper.pin_D7, OUTPUT); // Conecta o atuador ao GPIO 13, Pino D7
  pinMode(helper.pin_D8, OUTPUT); // Conecta o atuador ao GPIO 15, Pino D8

  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");

  if(strTopic == subsLedBoard){
    if(strMSG == "1"){
      digitalWrite(LED_BUILTIN, LOW);
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  if(strTopic == subsActuator1){ // Atuador GPIO 14, Pino D5
    if(strMSG == "1"){ 
      digitalWrite(helper.pin_D5, HIGH);
    }else{
      digitalWrite(helper.pin_D5, LOW);
    }
  }

  if(strTopic == subsActuator2){ // Atuador GPIO 12, Pino D6
    if(strMSG == "1"){
      digitalWrite(helper.pin_D6, HIGH);
    }else{
      digitalWrite(helper.pin_D6, LOW);
    }
  }

    if(strTopic == subsActuator3){ // Atuador GPIO 13, Pino D7
    if(strMSG == "1"){
      digitalWrite(helper.pin_D7, HIGH);
    }else{
      digitalWrite(helper.pin_D7, LOW);
    }
  }

    if(strTopic == subsActuator4){ // Atuador GPIO 14, Pino D8
    if(strMSG == "1"){
      digitalWrite(helper.pin_D8, HIGH);
    }else{
      digitalWrite(helper.pin_D8, LOW);
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
      //subscreve no tópico com nivel de qualidade QoS 1
      client.subscribe(subs.led_board, 1);       
      client.subscribe(subs.actuator1, 1); 
      client.subscribe(subs.actuator2, 1); 
      client.subscribe(subs.actuator3, 1); 
      client.subscribe(subs.actuator4, 1);             
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
  initSubTopics();
  client.subscribe(subs.led_board);  
  client.subscribe(subs.actuator1);
  client.subscribe(subs.actuator2);
  client.subscribe(subs.actuator3);
  client.subscribe(subs.actuator4);      
}

void setup() {
  Serial.begin(9600);
  initConfig();
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

  client.loop();  
}