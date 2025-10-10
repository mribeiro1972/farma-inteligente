#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Configurações do Wi-Fi
const char* ssid = "BUFUS REGULARIS"; // Substitua pelo SSID da sua rede Wi-Fi
const char* password = "BUFUS2112"; // Substitua pela senha da sua rede Wi-Fi

// Configurações do servidor
const char* serverName = "http://computacao.unir.br/marcello/post-data.php";
const char* apiKey = ""; // Substitua por uma chave secreta

// Configurações do sensor DHT11
#define DHTPIN 32 // Pino onde o DHT11 está conectado (ex.: GPIO 4)
#define DHTTYPE DHT11 // Tipo do sensor
DHT dht(DHTPIN, DHTTYPE);

// Intervalo de envio (em milissegundos)
const unsigned long interval = 30000; // Envia a cada 30 segundos
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();

  // Enviar dados a cada 'interval' milissegundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Verificar se está conectado ao Wi-Fi
    if (WiFi.status() == WL_CONNECTED) {
      // Ler dados do sensor DHT11
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      // Verificar se a leitura foi bem-sucedida
      if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Erro ao ler o sensor DHT11!");
        return;
      }

      Serial.print("Temperatura: ");
      Serial.print(temperature);
      Serial.print(" °C, Umidade: ");
      Serial.print(humidity);
      Serial.println(" %");

      // Preparar requisição HTTP
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Montar os dados a serem enviados
      String httpRequestData = "temperature=" + String(temperature) + "&humidity=" + String(humidity) + "&api_key=" + String(apiKey);

      // Enviar requisição POST
      int httpResponseCode = http.POST(httpRequestData);

      // Verificar resposta
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("Resposta do servidor: ");
        Serial.println(response);
      } else {
        Serial.print("Erro na requisição HTTP: ");
        Serial.println(httpResponseCode);
      }

      // Liberar recursos
      http.end();
    } else {
      Serial.println("Wi-Fi desconectado!");
    }
  }
}