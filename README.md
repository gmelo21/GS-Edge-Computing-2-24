# GS-Edge-Computing-2-24  

**Feito por Guilherme Melo (555310) e Matheus Gushi (556935).**  

## Descrição  

Este projeto implementa uma unidade de monitoramento para placas solares utilizando um ESP32. O sistema é capaz de calcular as condições climáticas, avaliar a produção de energia e checar a eficiência das placas solares. Os dados monitorados incluem temperatura, umidade, luminosidade, tensão e corrente. Além disso, o sistema pode se conectar ao **Energenius**, fornecendo informações em tempo real para uma análise mais precisa e integrada.  

## Funcionalidades  

- Monitoramento das condições climáticas (temperatura, umidade e luminosidade).  
- Cálculo da produção de energia através de sensores de tensão e corrente.  
- Verificação da eficiência das placas solares.  
- Exibição de dados no display LCD I2C para acompanhamento local.  
- Envio dos dados para um broker MQTT para integração com sistemas externos, como o **Energenius**.  
- Controle remoto de dispositivos conectados por comandos MQTT.  

## Montagem do Circuito  

- **Sensor de temperatura e umidade** (DHT22): conectado ao pino digital 15.  
- **Sensor de tensão**: conectado ao pino analógico 39.  
- **Sensor de corrente** (ACS712): conectado ao pino analógico 34.  
- **Sensor de luminosidade**: conectado ao pino analógico 35.  
- **Display LCD I2C**: conectado ao barramento I2C do ESP32.  
- **LED de controle**: conectado ao pino digital 2.  

## Configuração do Sistema  

### Conexão Wi-Fi e MQTT  

Certifique-se de configurar a conexão Wi-Fi e os tópicos MQTT no código:  
- **SSID**: `Wokwi-GUEST`  
- **Broker MQTT**: `191.235.32.167`  
- **Tópicos MQTT**:  
  - `/TEF/lampEDGE_GS/attrs`: Publicação dos dados de estado.  
  - `/TEF/lampEDGE_GS/attrs/l`: Luminosidade.  
  - `/TEF/lampEDGE_GS/attrs/h`: Umidade.  
  - `/TEF/lampEDGE_GS/attrs/t`: Temperatura.  
  - `/TEF/lampEDGE_GS/attrs/v`: Tensão.  
  - `/TEF/lampEDGE_GS/attrs/a`: Corrente.  

### Dependências  

- **Bibliotecas necessárias**:  
  - `WiFi`  
  - `PubSubClient`  
  - `DHTesp`  
  - `LiquidCrystal_I2C`  

Instale as bibliotecas mencionadas através do Gerenciador de Bibliotecas da Arduino IDE.  

## Operação  

1. **Inicialização**:  
   - O sistema conecta-se ao Wi-Fi e ao broker MQTT.  
   - Mensagens de inicialização são exibidas no display LCD I2C.  

2. **Monitoramento**:  
   - Os sensores coletam dados continuamente a cada 1 segundo.  
   - Os dados de tensão, corrente, temperatura, umidade e luminosidade são enviados ao broker MQTT.  
   - O display LCD alterna entre as leituras, exibindo informações como:  
     - Tensão (em volts) e corrente (em amperes).  
     - Temperatura (em °C) e umidade (em %).  
     - Luminosidade (em %).  

3. **Conexão ao Energenius**:  
   - Os dados coletados podem ser integrados ao **Energenius**, nosso aplicativo da Global Solution geral, para análise mais detalhada da eficiência das placas solares e condições climáticas.

## Requisitos  

### Hardware  

- ESP32  
- Sensor de Temperatura e Umidade (DHT22)  
- Sensor de Tensão  
- Sensor de Corrente (ACS712)  
- Sensor de Luminosidade  
- Display LCD 16x2 com interface I2C  
- Fonte de alimentação e cabos jumpers  

### Software  

- Arduino IDE  
- Bibliotecas mencionadas na seção de dependências  

## Contribuição  

Este projeto é mantido por Guilherme Melo e Matheus Gushi. Contribuições e melhorias são bem-vindas!  

**Repositório**: [GS-Edge-Computing-2-24](https://github.com/gmelo21/GS-Edge-Computing-2-24)  
