#include <ESP8266WiFi.h>

const char* ssid     = "<<FILLME>>";
const char* password = "<<FILLME>>";

// Wir setzen den Webserver auf Port 80
WiFiServer server(80);

String header;

String output5State = "off";
String output4State = "off";

const int output5 = 5;
const int output4 = 4;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WLAN verbunden.");
  Serial.println("IP Adresse: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Neuer Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /on") >= 0) {
              Serial.println("GPIO on");
              output5State = "on";
              output4State = "on";
              digitalWrite(LED_BUILTIN, LOW);
              digitalWrite(output5, HIGH);
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /off") >= 0) {
              Serial.println("GPIO off");
              output5State = "off";
              output4State = "off";
              digitalWrite(LED_BUILTIN, HIGH);
              digitalWrite(output5, LOW);
              digitalWrite(output4, LOW);
            }
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #333344; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #888899;}</style></head>");
            client.println("<body><h1>Heizung</h1>");
            // Zeige den aktuellen Status, und AN/AUS Buttons for GPIO 5  
            client.println("<p>GPIO State " + output5State + " " + output4State + "</p>");
            // wenn off, zeige den EIN Button       
            if (output5State=="off" && output4State=="off") {
              client.println("<p><a href=\"/on\"><button class=\"button\">EIN</button></a></p>");
            } else {
              client.println("<p><a href=\"/off\"><button class=\"button button2\">AUS</button></a></p>");
            }
            client.println("</body></html>");
            
            // Die HTTP-Antwort wird mit einer Leerzeile beendet
            client.println();
            // und wir verlassen mit einem break die Schleife
            break;
          } else { // falls eine neue Zeile kommt, lösche die aktuelle Zeile
            currentLine = "";
          }
        } else if (c != '\r') {  // wenn etwas kommt was kein Zeilenumbruch ist,
          currentLine += c;      // füge es am Ende von currentLine an
        }
      }
    }
    // Die Header-Variable für den nächsten Durchlauf löschen
    header = "";
    // Die Verbindung schließen
    client.stop();
    Serial.println("Client getrennt.");
    Serial.println("");
  }
}
