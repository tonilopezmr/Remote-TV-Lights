# 1 "/var/folders/1j/x8lsqxdj70535rn3mx9r1fjm0000gp/T/tmpbsy5q5mv"
#include <Arduino.h>
# 1 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/main.ino"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <constants.h>
#include <ArduinoJson.h>

#include "statics/index.html.gz.h"

IRrecv irrecv(IRIN);
decode_results results;
void setup();
void irReceiver();
void blinkLed();
void production();
void test();
void loop();
unsigned int getIrCodesSize();
void getIrCodes(String codes[]);
void setupCodes();
void setCodes(JsonArray codes);
void setWifiCredentials(String ssid, String password);
String getSsid();
String getWifiPassword();
void setupSettings();
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue);
template<typename T> bool setSetting(const String& key, T value);
template<typename T> bool setSetting(const String& key, unsigned int index, T value);
bool delSetting(const String& key);
bool delSetting(const String& key, unsigned int index);
bool hasSetting(const String& key);
bool hasSetting(const String& key, unsigned int index);
String getVisibility(bool isVisible);
String getBoardCode();
void toggleTransistor();
void toggleLed();
bool getTransistorStatus();
bool getLedStatus();
String int64toHEXString(uint64_t input);
void infoWifi();
void infoCallback(justwifi_messages_t code, char *parameter);
void notFound(AsyncWebServerRequest *request);
void onHome(AsyncWebServerRequest *request);
void webSetup();
DynamicJsonDocument getStatus();
void _wsConnected(uint32_t client_id);
void _wsParse(AsyncWebSocketClient *client, uint8_t *payload, size_t length);
void _onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void wsSend(uint32_t client_id, DynamicJsonDocument root);
void sendRestart(uint32_t client_id);
void sendCode(String code);
AsyncWebSocket wsSetup();
#line 17 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/main.ino"
void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn();
  delay(400);
  setupCodes();

  jw.setHostname(getBoardCode().c_str());
  jw.subscribe(infoCallback);
  jw.enableAP(false);
  jw.enableAPFallback(true);
  jw.enableSTA(true);

  String ssid = getSsid();
  String password = getWifiPassword();

  Serial.println("Conn to " + ssid);

  jw.addNetwork(ssid.c_str(), password.c_str());
  pinMode(BUTTON_PIN, INPUT);
  pinMode(TRANSISTOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  #ifdef TEST_ENVIRONMENT
    pinMode(2, OUTPUT);
  #endif

  webSetup();
  wsSetup();

  Serial.println("IR module");
}

void irReceiver()
{
  if (irrecv.decode(&results))
  {
    irrecv.resume();

    static unsigned long last_time = 0;
    if (millis() - last_time < IR_DEBOUNCE)
      return;
    last_time = millis();

    if (results.value < 1)
      return;
    if (results.decode_type < 1)
      return;
    if (results.bits < 1)
      return;
    char payload[32];
    snprintf_P(payload, sizeof(payload), PSTR("%u:%lu:%u"), results.decode_type, (unsigned long)results.value, results.bits);

    String codes[getIrCodesSize()];
    getIrCodes(codes);

    Serial.println(payload);
    sendCode(payload);

    for (unsigned int i = 0; i < sizeof(codes); i++)
    {
      if (codes[i] == payload)
      {
        Serial.println("Toggle transistor");
        toggleTransistor();
        break;
      }
    }

    irrecv.resume();
  }
}

void blinkLed()
{
  if (LED_BLINK_SUPPORT)
  {
    static unsigned long last_time = 0;
    if (millis() - last_time < LED_BLINK_TIME)
      return;
    last_time = millis();
    toggleLed();
  }
}

void production()
{
  irReceiver();







  jw.loop();
  blinkLed();
}

#ifdef TEST_ENVIRONMENT
void test()
{
  if (digitalRead(BUTTON_PIN))
  {
    digitalWrite(2, !digitalRead(2));
    delay(200);
  }
  else
  {
    toggleLed();
    toggleTransistor();
    delay(200);
    toggleLed();
    delay(200);
    toggleLed();
    toggleTransistor();
    delay(400);
  }
}
#endif

void loop()
{
#ifdef TEST_ENVIRONMENT
  test();
#else
  production();
#endif
}
# 1 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/domain.ino"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <constants.h>

#define NUM_CODES "num-codes"
#define SSID_PARAMETER "ssid"
#define PASSWORD_PARAMETER "password"
#define CODE_NAME_PREFIX "code"

std::vector<String> *_codes = new std::vector<String>();

unsigned int getIrCodesSize() {
  return _codes->size();
}

void getIrCodes(String codes[]) {
  std::copy(_codes->begin(), _codes->end(), codes);
}

void setupCodes() {
  setupSettings();

  if (hasSetting(NUM_CODES))
  {
    unsigned int numCodes = getSetting(NUM_CODES, "0").toInt();

    for (unsigned int i = 0; i < numCodes; i++)
    {
      if (hasSetting(CODE_NAME_PREFIX, i))
      {
        String code = getSetting(CODE_NAME_PREFIX, i, "");
        if (code.length() > 0)
        {
          Serial.println("Add code from EEPROM" + code);
          _codes->push_back(code);
        }
      }
    }
  }
}

void setCodes(JsonArray codes) {
    size_t numCodes = codes.size();
    if(numCodes > 0) {
        _codes->clear();
        setSetting(NUM_CODES, numCodes);

        for (unsigned int i = 0; i < numCodes; i++) {
            String codeName = CODE_NAME_PREFIX + String(i);
            String code = codes[i];

            Serial.println("Add code: " + code);
            _codes->push_back(code);
            setSetting(codeName, code);
        }
    }
}

void setWifiCredentials(String ssid, String password) {
      setSetting(SSID_PARAMETER, ssid);
      setSetting(PASSWORD_PARAMETER, password);
      Serial.println("Set WiFi credentials");
      ESP.restart();
}

String getSsid() {
  return getSetting(SSID_PARAMETER, DEFAULT_SSID);
}

String getWifiPassword() {
  return getSetting(PASSWORD_PARAMETER, DEFAULT_WIFI_PASSWORD);
}
# 1 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/settings.ino"
#include "Embedis.h"
#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

void setupSettings() {
    EEPROMr.begin(SPI_FLASH_SEC_SIZE);

    Embedis::dictionary( F("EEPROM"),
        SPI_FLASH_SEC_SIZE,
        [](size_t pos) -> char { return EEPROMr.read(pos); },
        [](size_t pos, char value) { EEPROMr.write(pos, value); },
        []() { EEPROMr.commit(); }
    );

}

template<typename T> String getSetting(const String& key, T defaultValue) {
    String value;
    if (!Embedis::get(key, value)) value = String(defaultValue);
    return value;
}

template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue) {
    return getSetting(key + String(index), defaultValue);
}

template<typename T> bool setSetting(const String& key, T value) {
    return Embedis::set(key, String(value));
}

template<typename T> bool setSetting(const String& key, unsigned int index, T value) {
    return setSetting(key + String(index), value);
}

bool delSetting(const String& key) {
    return Embedis::del(key);
}

bool delSetting(const String& key, unsigned int index) {
    return delSetting(key + String(index));
}

bool hasSetting(const String& key) {
    return getSetting(key, "").length() != 0;
}

bool hasSetting(const String& key, unsigned int index) {
    return getSetting(key, index, "").length() != 0;
}
# 1 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/utils.ino"
#include <Arduino.h>
#include <constants.h>

String getVisibility(bool isVisible)
{
    String visibility = "hidden";
    if (isVisible)
    {
        visibility = "visible";
    }
    return visibility;
}

String getBoardCode()
{
    char buffer[20];
    snprintf_P(buffer, sizeof(buffer), PSTR("%s-%06X"), "KARUMI", ESP.getChipId());
    return String(buffer);
}

void toggleTransistor()
{
    digitalWrite(TRANSISTOR_PIN, !getTransistorStatus());
}

void toggleLed()
{
    digitalWrite(LED_PIN, !getLedStatus());
}

bool getTransistorStatus()
{
    return digitalRead(TRANSISTOR_PIN);
}

bool getLedStatus()
{
    return digitalRead(LED_PIN);
}

String int64toHEXString(uint64_t input)
{
    char buf[8 * sizeof(input) + 1];
    char *str = &buf[sizeof(buf) - 1];

    *str = '\0';

    do
    {
        char c = input % HEX;
        input /= HEX;

        *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while (input);

    return String(str);
}

void infoWifi()
{

    if (WiFi.isConnected())
    {

        uint8_t *bssid = WiFi.BSSID();

        Serial.printf("[WIFI] MODE STA -------------------------------------\n");
        Serial.printf("[WIFI] SSID  %s\n", WiFi.SSID().c_str());
        Serial.printf("[WIFI] BSSID %02X:%02X:%02X:%02X:%02X:%02X\n",
                      bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        Serial.printf("[WIFI] CH    %d\n", WiFi.channel());
        Serial.printf("[WIFI] RSSI  %d\n", WiFi.RSSI());
        Serial.printf("[WIFI] IP    %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WIFI] MAC   %s\n", WiFi.macAddress().c_str());
        Serial.printf("[WIFI] GW    %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("[WIFI] MASK  %s\n", WiFi.subnetMask().toString().c_str());
        Serial.printf("[WIFI] DNS   %s\n", WiFi.dnsIP().toString().c_str());
#if defined(ARDUINO_ARCH_ESP32)
        Serial.printf("[WIFI] HOST  %s\n", WiFi.getHostname());
#else
        Serial.printf("[WIFI] HOST  %s\n", WiFi.hostname().c_str());
#endif
        Serial.printf("[WIFI] ----------------------------------------------\n");
    }

    if (WiFi.getMode() & WIFI_AP)
    {

        Serial.printf("[WIFI] MODE AP --------------------------------------\n");
        Serial.printf("[WIFI] SSID  %s\n", jw.getAPSSID().c_str());
        Serial.printf("[WIFI] IP    %s\n", WiFi.softAPIP().toString().c_str());
        Serial.printf("[WIFI] MAC   %s\n", WiFi.softAPmacAddress().c_str());
        Serial.printf("[WIFI] ----------------------------------------------\n");
    }
}

void infoCallback(justwifi_messages_t code, char *parameter)
{



    if (code == MESSAGE_TURNING_OFF)
    {
        Serial.printf("[WIFI] Turning OFF\n");
    }

    if (code == MESSAGE_TURNING_ON)
    {
        Serial.printf("[WIFI] Turning ON\n");
    }



    if (code == MESSAGE_SCANNING)
    {
        Serial.printf("[WIFI] Scanning\n");
    }

    if (code == MESSAGE_SCAN_FAILED)
    {
        Serial.printf("[WIFI] Scan failed\n");
    }

    if (code == MESSAGE_NO_NETWORKS)
    {
        Serial.printf("[WIFI] No networks found\n");
    }

    if (code == MESSAGE_NO_KNOWN_NETWORKS)
    {
        Serial.printf("[WIFI] No known networks found\n");
    }

    if (code == MESSAGE_FOUND_NETWORK)
    {
        Serial.printf("[WIFI] %s\n", parameter);
    }



    if (code == MESSAGE_CONNECTING)
    {
        Serial.printf("[WIFI] Connecting to %s\n", parameter);
    }

    if (code == MESSAGE_CONNECT_WAITING)
    {

    }

    if (code == MESSAGE_CONNECT_FAILED)
    {
        Serial.printf("[WIFI] Could not connect to %s\n", parameter);
    }

    if (code == MESSAGE_CONNECTED)
    {
        infoWifi();
    }

    if (code == MESSAGE_DISCONNECTED)
    {
        Serial.printf("[WIFI] Disconnected\n");
    }



    if (code == MESSAGE_ACCESSPOINT_CREATED)
    {
        infoWifi();
    }

    if (code == MESSAGE_ACCESSPOINT_DESTROYED)
    {
        Serial.printf("[WIFI] Disconnecting access point\n");
    }

    if (code == MESSAGE_ACCESSPOINT_CREATING)
    {
        Serial.printf("[WIFI] Creating access point\n");
    }

    if (code == MESSAGE_ACCESSPOINT_FAILED)
    {
        Serial.printf("[WIFI] Could not create access point\n");
    }



    if (code == MESSAGE_WPS_START)
    {
        Serial.printf("[WIFI] WPS started\n");
    }

    if (code == MESSAGE_WPS_SUCCESS)
    {
        Serial.printf("[WIFI] WPS succeded!\n");
    }

    if (code == MESSAGE_WPS_ERROR)
    {
        Serial.printf("[WIFI] WPS failed\n");
    }



    if (code == MESSAGE_SMARTCONFIG_START)
    {
        Serial.printf("[WIFI] Smart Config started\n");
    }

    if (code == MESSAGE_SMARTCONFIG_SUCCESS)
    {
        Serial.printf("[WIFI] Smart Config succeded!\n");
    }

    if (code == MESSAGE_SMARTCONFIG_ERROR)
    {
        Serial.printf("[WIFI] Smart Config failed\n");
    }
};
# 1 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/web.ino"
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer * _server;
char last_modified[50];

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/html", "Not found");
}

void onHome(AsyncWebServerRequest *request) {

    if (request->header("If-Modified-Since").equals(last_modified)) {
        request->send(304);

    } else {

        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);

        response->addHeader("Content-Encoding", "gzip");

        response->addHeader("Last-Modified", last_modified);
        request->send(response);
    }
}

AsyncWebServer * webServer() {
    return _server;
}

void webSetup() {
  _server = new AsyncWebServer(80);

  sprintf(last_modified, "%s %s GMT", __DATE__, __TIME__);
  _server->rewrite("/", "/index.html");
  _server->on("/index.html", HTTP_GET, onHome);
  _server->onNotFound(notFound);
  _server->begin();
}
# 1 "/Users/toni/Documents/electronics/Remote-TV-Lights/software/src/ws.ino"
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <algorithm>
#include <lib/WebSocketIncommingBuffer.h>

AsyncWebSocket _ws("/ws");
std::vector<uint32_t> _client_ids;

DynamicJsonDocument getStatus()
{
    const size_t capacity = JSON_OBJECT_SIZE(2);
    DynamicJsonDocument root(capacity + 50);

    root["relay"] = getTransistorStatus();
    String boardName = getBoardCode();
    Serial.println("Boardname: " + boardName);
    root["boardName"] = boardName;

    return root;
}

void _wsConnected(uint32_t client_id)
{
    DynamicJsonDocument root = getStatus();

    _client_ids.push_back(client_id);
    wsSend(client_id, root);
}

void _wsParse(AsyncWebSocketClient *client, uint8_t *payload, size_t length)
{
    uint32_t client_id = client->id();

    if ((length == 0) || (length == 1))
    {
        return;
    }

    if ((length == 3) && (strcmp((char *)payload, "{}") == 0))
    {
        return;
    }

    DynamicJsonDocument root(512);
    auto error = deserializeJson(root, (char *)payload);
    if (error)
    {
        Serial.print("[WEBSOCKET] JSON parsing error with code: ");
        Serial.println(error.c_str());
        return;
    }

    const char *action = root["action"];
    if (action)
    {
        Serial.printf("[WEBSOCKET] Requested action: %s \n", action);

        if (strcmp(action, "wifi") == 0)
        {
            String ssid = root["ssid"];
            String password = root["password"];
            sendRestart(client_id);
            setWifiCredentials(ssid, password);
        }
        else if (strcmp(action, "codes") == 0)
        {
            JsonArray codes = root["codes"];
            setCodes(codes);
        }
        else if (strcmp(action, "relay") == 0)
        {
            toggleTransistor();
            wsSend(client_id, getStatus());
        }
    }
}

void _onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        client->_tempObject = nullptr;
        IPAddress ip = client->remoteIP();
        Serial.printf("New client id: %u connected IP %d.%d.%d.%d", client->id(), ip[0], ip[1], ip[2], ip[3]);
        _wsConnected(client->id());
        client->_tempObject = new WebSocketIncommingBuffer(_wsParse, true);
    }
    else if (type == WS_EVT_DISCONNECT)
    {

        _client_ids.erase(std::remove(_client_ids.begin(), _client_ids.end(), client->id()), _client_ids.end());
        Serial.printf("Client %u disconnected", client->id());

        if (client->_tempObject)
        {
            delete (WebSocketIncommingBuffer *)client->_tempObject;
        }
    }
    else if (type == WS_EVT_ERROR)
    {
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
    }
    else if (type == WS_EVT_DATA)
    {
        Serial.println("Event data comming");
        if (!client->_tempObject)
            return;
        WebSocketIncommingBuffer *buffer = (WebSocketIncommingBuffer *)client->_tempObject;
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        buffer->data_event(client, info, data, len);
    }
}

void wsSend(uint32_t client_id, DynamicJsonDocument root)
{
    Serial.printf("Send message to %u \n", client_id);
    AsyncWebSocketClient *client = _ws.client(client_id);
    if (client == nullptr)
        return;

    size_t len = measureJson(root);
    AsyncWebSocketMessageBuffer *buffer = _ws.makeBuffer(len);

    if (buffer)
    {
        serializeJson(root, reinterpret_cast<char *>(buffer->get()), len + 1);
        client->text(buffer);
    }
}

void sendRestart(uint32_t client_id)
{
    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument root(capacity + 30);

    root["restart"] = true;

    wsSend(client_id, root);
}

void sendCode(String code)
{
    if (_client_ids.size() == 0)
        return;

    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity + 40);
    doc["code"] = code;

    for (unsigned int i = 0; i < _client_ids.size(); i++)
    {
        uint32_t client_id = _client_ids[i];
        wsSend(client_id, doc);
    }
}

AsyncWebSocket wsSetup()
{
    _ws.onEvent(_onEvent);
    webServer()->addHandler(&_ws);
    return _ws;
}