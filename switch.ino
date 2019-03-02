
#include <ESP8266WiFi.h>

const char* ssid     = "2.4-1707 Sassafras";
//"PORSCHE"
const char* password = "CXNK0030C4DB";
//"12345678"

WiFiServer server(80);

String header;

String output5State = "used";
String output4State = "used";

const int output5 = 5;
const int output4 = 4;

void setup() {
  Serial.begin(115200);

  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
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
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   
  if (client) {                             
    Serial.println("New Client in.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/empty") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "empty";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "used";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "empty";
              digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "used";
              digitalWrite(output4, HIGH);
            }
 
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("body {background-color: powderblue;}");
            client.println(".button { background-color: Tomato; border: none; color: white; width: 100px; height: 50;border-radius: 8px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: Gray;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ECE 387 Wireless Comunication</h1>");
            client.println("<h2>Locker Room</h2>");
            client.println("<hr>");
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p style = color:green>01: " + output5State + " Password:</p>");
            client.println("<input type=text id:psw1>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="used") {
              client.println("<p><a href=\"/5/empty\"><button onclick= myFunction(); class=\"button\">Empty</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/on\"><button onclick= myFunction(); class=\"button button2\">Used</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p style = color:yellow>02: " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="used") {
              client.println("<p><a href=\"/4/off\"><button class=\"button\">Empty</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/on\"><button class=\"button button2\">Used</button></a></p>");
            }
            client.println("function myFunction() {var x = document.getElementById(\"myText\").value}");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
