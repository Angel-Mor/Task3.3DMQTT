#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

#define EchoPin 2
#define TrigPin 3
#define LedPin 4  // Define pin for LED

char ssid[] = "Connect Next";    // your network SSID
char pass[] = "12345678";    // your network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.emqx.io";
int        port     = 1883;
const char topic[]  = "SIT210/wave";
const char patTopic[] = "SIT210/pat"; // New pat topic

long Duration;
int Distance;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TrigPin, OUTPUT); 
  pinMode(EchoPin, INPUT); 
  pinMode(LedPin, OUTPUT);  // Set the LED pin as output

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

  // Subscribe to the pat topic
  mqttClient.subscribe(patTopic); // Subscribing to the new pat topic
  Serial.println("Subscribed to SIT210/pat topic");
}

void loop() {
  // MQTT keep-alive
  mqttClient.poll();

  // Measure distance using ultrasonic sensor
  digitalWrite(TrigPin, LOW);
  delay(2);
  digitalWrite(TrigPin, HIGH);
  delay(10);
  digitalWrite(TrigPin, LOW);
  Duration = pulseIn(EchoPin, HIGH);
  Distance = Duration * 0.034 / 2;
  Serial.print("Distance: ");  
  Serial.println(Distance);
  
  // Check distance and blink LED if distance is <= 20 cm
  if (Distance <= 20) {
    Serial.print("Distance is less than or equal to 20 cm, blinking LED...");
    
    // Blink the LED 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(LedPin, HIGH);
      delay(200);
      digitalWrite(LedPin, LOW);
      delay(200);
    }
    
    // Send message to MQTT topic
    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    mqttClient.beginMessage(topic);
    mqttClient.print("Message from Angel mor");
    mqttClient.print(count);
    mqttClient.endMessage();
    count++;
  }

  // Check if we received any message on the 'pat' topic
  int messageSize = mqttClient.parseMessage();
  if (messageSize && String(mqttClient.messageTopic()) == patTopic) {
    Serial.print("Received message on topic: ");
    Serial.println(patTopic);

    // Handle 'pat' topic message by blinking the LED 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(LedPin, HIGH);
      delay(200);
      digitalWrite(LedPin, LOW);
      delay(200);
    }
  }

  delay(1000);
}
