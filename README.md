# IoT Projects
Este repositório irá armazenar projetos de implementações de dispositivos, firmwares e seus esquemas eletrônicos, que utilizam seus microcontroladores como clientes de serviço MQTT.
O microcontrolador conectará a rede WIFI e a um Servidor MQTT. Ele percorrerá, quando houver, todos os sensores e atuadores conectados ao dispositivo e publicará seus valores no Servidor MQTT.
As mensagens publicadas pelo dispositivo podem ser enviadas nos seguintes formatos:
* simple (Valor bruto)
* JSON

## Microcontroladores testados
* ESP8266 (NodeMCU V3 Lolin)

### Estrutura dos diretórios:
O nome do diretório com o projeto do dispositivo indica a controladora, sensores ou acionadores de atuador utilizados e versão com separadores distintos. Por exemplo, no diretório ```ESP8266_NodeMCU_V3_Lolin]&(DHT11)%{V1.2}```, pode-se extrair as seguintes informações:
1. Controladora e modelo da Placa: ESP8266 (NodeMCU V3 Lolin)
2. Nome do sensor utilizado: DHT11
3. Versão do firmware: V1.2

### Versões: 
Os projetos estão dividios em versões para a realização de testes modulares. Abaixo a descrição de cada versão:  
- [x] **VTeste**:  
Este firmware é ideal para realizar testes rápidos de conexão com a rede wifi e com o servidor Broker, sem que haja a necessidade de conectar sensores para verificar se esta enviando mensagens corretamente.
O dispositivo simula um sensor de temperatura e umidade e atribui valores gerados aleatoriamente para substituir as medições.  
- [x] **V1.1**:  
Todos os códigos fonte de versão ```1.1``` referem-se a implementações de dispositivos que possuem apenas o sensor ou acionador de atuador conectado a controladora.
Esta versão possibilita realizar testes simples para verificações rápidas do funcionamento dos componentes.  
- [x] **V1.2**:  
A partir da versão ```1.2``` o dispositivo terá suporte de um módulo que permite o registro de valores de data e hora no decorrer do tempo. Exemplo: Módulo RTC.  
As mensagens enviadas com os dados dos sensores terá agregadas registros do momento que foi captadas.  
- [ ] **V1.3**:  
A partir da versão ```1.3``` é adicionado ao dispositivo algum recurso de memória não volátil para auxiliar o firmware em algumas tarefas. Exemplo: SD Card.  
- [ ] **V2.0**:  
Duas cabeças pensam melhor que uma. A versão ```2.0``` refere-se a implementação de um dispositivo que realiza uma comunicação estável entre dois controladores. 
Nesta versão é possível realizar testes rápidos de comunicação entre microcontroladoras.  
- [ ] **V2.1**:  
Dispositivo que usa multicontroladores com sensores ou acionador de atuador conectado.  
- [ ] **V2.2**:  
Dispositivo que usa multicontroladores com suporte de um módulo que permite o registro de valores de data e hora no decorrer do tempo. Exemplo: Módulo RTC.  
- [ ] **V2.3**:  
Dispositivo que usa multicontroladores com a adição de algum recurso de memória não volátil para auxiliar o firmware em algumas tarefas. Exemplo: SD Card.  

	
### Estrutura dos tópicos e mensagem
O tópico é montado a partir de uma estrutura pré-definida e dividida em barras '/' no formato String. Por exemplo, no tópico "```Home/ESP8266/simple/umidade/DHT11```":
1. Local do dispositivo: Home
2. Nome do dispositivo: ESP8266
3. Formato da mensagem enviada: simple
4. Tipo do sensor: umidade
5. Nome do sensor: DHT11

Caso o tópico esteja com o formato de mensagem em ```JSON``` ao invés de ```simple```, a mensagem enviada será um JSON com as informações que estão no tópico e a inclusão do dado medido. 
Por exemplo, no tópico ```Home/ESP8266/JSON/umidade/DHT11```, a mensagem enviada está no seguinte formato:  
  
{  
&nbsp;&nbsp;&nbsp;&nbsp; 'local' : 'Home',  
&nbsp;&nbsp;&nbsp;&nbsp; 'device_name' : 'ESP8266',  
&nbsp;&nbsp;&nbsp;&nbsp; 'sensor_name' : 'DHT11',  
&nbsp;&nbsp;&nbsp;&nbsp; 'type' : 'umidade',  
&nbsp;&nbsp;&nbsp;&nbsp; 'data' : '16.00',  
&nbsp;&nbsp;&nbsp;&nbsp; 'date_time' : '14/5/2023 10:16:46'  
}  

