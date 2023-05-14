# IoT Firmwares
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
- [x] **VTeste**
Este firmware é ideal para realizar testes rápidos de conexão com a rede wifi e com o servidor Borker, sem que haja a necessidade de conectar sensores para verificar se esta enviando mensagens corretamente.
O dispositivo simula um sensor de temperatura e umidade e atribui valores gerados aleatoriamente para substituir as medições.
- [x] **V1.1**
Todos os códigos fonte de versão ```1.1``` referem-se a implementações de dispositivos que possuem apenas o sensor ou acionador de atuador conectado.
Esta versão possibilita realizar testes simples para verificações rápidas do funcionamento dos componentes.
- [x] **V1.2**
RTC
- [ ] **V1.3**
SD Card
- [ ] **V2.0**
Duas cabeças pensam melhor que uma. A versão ```2.0``` refere-se a implementação de um dispositivo que realiza uma comunicação estável entre dois controladores. 
Nesta versão é possível realizar testes rápidos de comunicação entre microcontroladoras.
- [ ] **V2.1**
Dispositivo de multicontroladores com sensores ou acionador de atuador conectado.
- [ ] **V2.2**
Dispositivo de multicontroladores com sensores ou acionador de atuador conectado e RTC
- [ ] **V2.3**
Dispositivo de multicontroladores com sensores ou acionador de atuador conectado e RTC SD Card
	
### Estrutura dos tópicos e mensagem
O tópico é montado a partir de uma estrutura pré-definida e dividida em barras '/' no formato String. Por exemplo, no tópico "```Home/ESP8266/simple/umidade/DHT11```":
1. Local do dispositivo: Home
2. Nome do dispositivo: ESP8266
3. Formato da mensagem enviada: simple
4. Tipo do sensor: umidade
5. Nome do sensor: DHT11

Caso o tópico esteja com o formato de mensagem em JSON ao invés de simple, a mensagem enviada será um JSON com as informações que estão no tópico e a inclusão do dado medido. 
Por exemplo, no tópico ```Home/ESP8266/JSON/umidade/DHT11```, a mensagem enviada está no seguinte formato:
{
	
	
	
	
}
