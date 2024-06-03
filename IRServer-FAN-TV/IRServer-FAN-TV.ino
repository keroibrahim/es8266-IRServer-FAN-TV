/*
 * IRremoteESP8266: IRServer - demonstrates sending IR codes controlled from a webserver
 * Version 0.3 May, 2019
 * Version 0.2 June, 2017
 * Copyright 2015 Mark Szabo
 * Copyright 2019 David Conran
 *
 * An IR LED circuit *MUST* be connected to the ESP on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */
#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#endif  // ESP8266
#if defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#endif  // ESP32
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

const char* kSsid = "kiromar";
const char* kPassword = "ke31ro10";
MDNSResponder mdns;

#if defined(ESP8266)
ESP8266WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "koko"
#endif  // ESP8266
#if defined(ESP32)
WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "esp32"
#endif  // ESP32

const uint16_t kIrLed = 4;  // ESP GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head>" \
                  "<title>" HOSTNAME " Remote Control</title>" \
                  "<meta http-equiv=\"Content-Type\" " \
                    "content=\"text/html;charset=utf-8\">" \
                  "<meta name=\"viewport\" content=\"width=device-width," \
                    "initial-scale=1.0,minimum-scale=1.0," \
                    "maximum-scale=1.0,user-scalable=no\">" \
                  "<style>" \
                    "body {" \
                      "background-color: #333;" \
                      "color: #fff;" \
                      "font-family: Arial, sans-serif;" \
                      "text-align: center;" \
                    "}" \
                    ".remote-button {" \
                      "display: inline-block;" \
                      "margin: 10px;" \
                      "padding: 15px 30px;" \
                      "background-color: #222;" \
                      "color: #fff;" \
                      "border: none;" \
                      "border-radius: 10px;" \
                      "text-align: center;" \
                      "text-decoration: none;" \
                      "font-size: 16px;" \
                      "cursor: pointer;" \
                    "}" \
                    ".remote-button.red {" \
                      "background-color: #ff0000;" \
                    "}" \
                    ".remote-button.green {" \
                      "background-color: #00FF00;" \
                    "}" \
                    ".remote-button.blue {" \
                      "background-color: #0000FF;" \
                    "}" \
                  "</style>" \
                "</head>" \
                "<body>" \
                  "<h1>Remote Control for " HOSTNAME "</h1>" \
                  "<a class=\"remote-button green\" href=\"ir?code=3458\">ON/speed</a>" \
                  "<a class=\"remote-button red\" href=\"ir?code=3457\">OFF</a>" \
                  "<a class=\"remote-button blue\" href=\"ir?code=3472\">OSC</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=3460\">NATURAL</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=3464\">TIMER</a>" \
                  "<h1>Remote Control for TV</h1>" \
                  "<a class=\"remote-button red\" href=\"ir?code=551489775\">LG/POWER</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551538735\">INPUT</a>" \
                  "<a class=\"remote-button green\" href=\"ir?code=551528790\">ENG/SAVE</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551502015\">VOL+</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551534655\">VOL-</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551522415\">MUTE</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551486205\">UP</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551518845\">DOWN</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551510175\">RIGHT</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551542815\">LEFT</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551494365\">OK</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551490795\">BACK</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551541285\">EXIT</a>" \
                  "<a class=\"remote-button blue\" href=\"ir?code=551535165\">HOME</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551500740\">3D</a>" \
                  "<a class=\"remote-button\" href=\"ir?code=551525985\">RATIO</a>" \
                "</body>" \
              "</html>");
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);

#if SEND_NEC
      irsend.sendNEC(code, 32);
#endif  // SEND_NEC

#if SEND_SYMPHONY
      irsend.sendSymphony(code, 12);
#endif  // SEND_SYMPHONY
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(kSsid, kPassword);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(kSsid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

#if defined(ESP8266)
  if (mdns.begin(HOSTNAME, WiFi.localIP())) {
#else  // ESP8266
  if (mdns.begin(HOSTNAME)) {
#endif  // ESP8266
    Serial.println("MDNS responder started");
    // Announce http tcp service on port 80
    mdns.addService("http", "tcp", 80);
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
#if defined(ESP8266)
  mdns.update();
#endif
  server.handleClient();
}
