//#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>

#include <TimeLib.h>
#include <WidgetRTC.h>

#define SONOFF_BUTTON      0
#define SONOFF_INPUT      14
#define SONOFF_LED        13
#define SONOFF_RELAY_PIN  12

// Your WiFi credentials.
// Set password to "" for open networks.
#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

#define HOSTNAME "Timer_01"

const char* ssid = STASSID;
const char* pass = STAPSK;
char auth[] = "";

BlynkTimer timer;
WidgetRTC rtc;

WidgetLED statusLED(V10);
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#E6E600"
#define BLYNK_ORANGE    "#E68A00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"

bool relayState = LOW;
bool manualButton = 0;
bool alertsOn = 0;
long lastDebounceTime = 0;
long debounceDelay = 150;

void turnOn()
{
  digitalWrite(SONOFF_RELAY_PIN, HIGH);
  digitalWrite(SONOFF_LED, LOW);
  Blynk.virtualWrite(V1, 1);
  if(alertsOn)
    Blynk.notify("{DEVICE_NAME} is on!");
  relayState = HIGH;
}

void turnOff()
{
  digitalWrite(SONOFF_RELAY_PIN, LOW);
  digitalWrite(SONOFF_LED, HIGH);
  Blynk.virtualWrite(V1, 0);
  statusLED.off();
  if(alertsOn)
    Blynk.notify("{DEVICE_NAME} is off!");
  relayState = LOW;
}

ICACHE_RAM_ATTR void notifyOnButtonPress()
{
  // Invert state, since button is "Active LOW"
  if(digitalRead(SONOFF_BUTTON) == LOW)
  {
    if((millis() - lastDebounceTime) > debounceDelay)
    {
      lastDebounceTime = millis();
      manualButton = 1;
      #ifdef BLYNK_PRINT
      Serial.println("Button is pressed.");
      // Note:
      // We allow 1 notification per 5 seconds for now.
      Blynk.notify("Yaaay... {DEVICE_NAME} button is pressed!");
      #endif
      if(relayState == LOW)
      {
        turnOn();
        Blynk.setProperty(V10, "color", BLYNK_GREEN);
        statusLED.on();
      }
      else
      {
        turnOff();
      }
    }
  }
}

void requestTime()
{
  Blynk.sendInternal("rtc", "sync");
}

BLYNK_WRITE(V0) // Alerts On or Off
{
  if (param.asInt() == 1)
    alertsOn = 1;
  else
    alertsOn = 0;
}

BLYNK_WRITE(V1) // LED Button Press
{       
    if (param.asInt() == 1)
    {
      turnOn();
      Blynk.setProperty(V10, "color", BLYNK_ORANGE);
      statusLED.on();
    } 
    else 
    {
      turnOff();
    }
}

BLYNK_WRITE(V2) {
  TimeInputParam t(param);
  int today = (weekday(now())+6)%7;  //Time library starts week on Sunday, Blynk on Monday  
  if(today == 0)
    today = 7;
  unsigned int nowseconds = now() % 86400;
  unsigned int startseconds = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);  
  unsigned int stopseconds = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
  if(t.isWeekdaySelected(today))
  { 
    if(nowseconds >= startseconds - 31 && nowseconds <= startseconds + 31 )
    {
      Blynk.setProperty(V10, "color", BLYNK_BLUE);
      statusLED.on();
      turnOn();
    }                  
    if(nowseconds >= stopseconds - 31 && nowseconds <= stopseconds + 31 )
    {
      turnOff();
    }
    if (t.isStartSunrise())
    {
      Blynk.setProperty(V10, "color", BLYNK_BLUE);
      statusLED.on();
      turnOn();
    }
    if (t.isStartSunset())
    {
      Blynk.setProperty(V10, "color", BLYNK_BLUE);
      statusLED.on();
      turnOn();
    }
    if (t.isStopSunrise())
    {
      turnOff();
    }
    if (t.isStopSunset())
    {
      turnOff();
    }
  }
}

void setup() {
  pinMode(SONOFF_LED, OUTPUT);
  digitalWrite(SONOFF_LED, HIGH);

  pinMode(SONOFF_RELAY_PIN, OUTPUT);
  digitalWrite(SONOFF_RELAY_PIN, LOW);

  pinMode(SONOFF_BUTTON, INPUT);

  #ifndef BLYNK_PRINT
  Serial.begin(115200); //Not really needed, Sets 0 the serial debug port.
  #endif
  Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,160), 8080);  //connects to the blynk server using the credentals from above.

  rtc.begin();
  Blynk.syncAll();

  ArduinoOTA.setHostname(HOSTNAME);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  #ifndef BLYNK_PRINT
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
  
  statusLED.off();

  timer.setInterval(3600000L, requestTime);

  attachInterrupt(digitalPinToInterrupt(SONOFF_BUTTON), notifyOnButtonPress, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();

  timer.run();
  
  ArduinoOTA.handle();
}
