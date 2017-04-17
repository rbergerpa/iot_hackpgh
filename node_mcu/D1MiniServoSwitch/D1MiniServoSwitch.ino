#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wifi Connection:
const char* myHostname = "Main Light"; //CAN BE WHATEVER
//
const char* ssid = "HackPGH"; //YOUR SSID
//
const char* password = "xxx"; // YOUR PASSWORD (KEEP "QUOTES" ON ALL THESE ITEMS, BY THE WAY)

//Sensor Configuration:
const int SERVO_PIN = 4; // THIS IS PIN D2.  CHOSEN BECAUSE IT DOESN'T INTERFERE WITH THE BOOT SEQUENCE ON THE D1
Servo myservo;
int sensor_read_new = 0;
int sensor_read_old = 0;
int sensor_update_seconds = 600;
int sensor_update_millis = sensor_update_seconds * 1000;
int servo_open_pos = 140;     //YOU MAY HAVE TO TWEAK THESE VALUES, BUT THEY WORKED FOR ME
int servo_neutral_pos = 100;   //YOU MAY HAVE TO TWEAK THESE VALUES, BUT THEY WORKED FOR ME
int servo_closed_pos = 60;    //YOU MAY HAVE TO TWEAK THESE VALUES, BUT THEY WORKED FOR ME

// MQTT Server address and configuration:
const char* mqtt_server = "10.20.30.10"; //CHANGE TO WHAT YOUR MQTT SERVER IS RUNNING ON.  MAY HAVE TO SET STATIC IP FOR MOSQUITTO SERVER IN ROUTER SETTINGS!!
const char* mqtt_client_id = "main_light"; //HOW THE MQTT CLIENT APPEARS TO THE SERVER
const char* mqtt_user = ""; // USER NAME IF YOUR MQTT SERVER IS PASSWORD PROTECTED, IF NOT USE ""
const char* mqtt_password = ""; // PASSWORD IF YOUR MQTT SERVER IS PASSWORD PROTECTED, IF NOT USE ""
const char* mqtt_command_topic = "/lights/main/set"; //CAN YOU GUESS WHERE MY LIGHT WAS, CHANGE TO THE TOPIC YOU WISH TO USE TO CONTROL THE SERVO
const char* mqtt_state_topic = "/lights/main/state"; //I WAS EVENTUALLY GOING TO PUT A SENSOR ON THE WEMOS AS WELL AND RECORD THE ACTUAL STATE, BUT THAT'S IN THE FUTURE, THIS CODE DOESN'T MATTER
const char* mqtt_all_topic = "/lights/all/set";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); //MAY NEED TO CHANGE PORT IF YOU'RE NOT RUNNING ON 1883
  client.setCallback(callback);

}
void setup_wifi() {
  delay(2);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println(ssid);
  WiFi.hostname(myHostname);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  int val;
  if ((char)payload[0] == '1') {
    myservo.attach(SERVO_PIN);
    myservo.write(servo_open_pos);
    delay(200);
    myservo.write(servo_neutral_pos);
    delay(200);
    myservo.detach();
    client.publish(mqtt_state_topic, "1");
  }
  if ((char)payload[0] == '0') {
    myservo.attach(SERVO_PIN);
    myservo.write(servo_closed_pos);
    delay(200);
    myservo.write(servo_neutral_pos);
    delay(200);
    myservo.detach();
    client.publish(mqtt_state_topic, "0");
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_state_topic, "Connected");
      // ... and resubscribe
      client.subscribe(mqtt_command_topic);
      client.subscribe(mqtt_all_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > sensor_update_millis) {
    lastMsg = now;
    ++value;
  }

}

