#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

char ssid[] = "Connect Next";    // your network SSID
char pass[] = "12345678";    // your network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.emqx.io";
int        port     = 1883;
const char topic[]  = "SIT210/wave";
const char patTopic[] = "SIT210/pat"; // New pat topic

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  while (!Serial);

  // Attempt to connect to WiFi network
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println("You're connected to the network");

  // Connect to MQTT broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println("You're connected to the MQTT broker!");

  // Subscribe to topics
  mqttClient.subscribe(topic);
  mqttClient.subscribe(patTopic); // Subscribing to the new pat topic
  Serial.println("Subscribed to topics SIT210/wave and SIT210/pat");

  Serial.print("Waiting for messages on topics: ");
  Serial.println(topic);
}

void loop() {
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    while (mqttClient.available()) {
      Serial.print((char)mqttClient.read());
    }
    Serial.println();

    if (String(mqttClient.messageTopic()) == patTopic) {
      // Handle the 'pat' topic message with an action (like blinking the LED)
      Serial.println("Handling message from SIT210/pat");
      for (int i = 0; i < 3; i++) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
      }
    }

    // Publish the name "Angel mor" to the topic SIT210/wave
    mqttClient.beginMessage(topic);
    mqttClient.print("Message from Angel mor");
    mqttClient.endMessage();

    delay(1000);
  }
}
