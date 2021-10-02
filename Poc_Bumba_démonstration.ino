
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define CUSTOM_SETTINGS
#define INCLUDE_MUSIC_MODULE
#include <DabbleESP32.h>
#define WLAN_SSID       "FpooiWifi"                      
#define WLAN_PASS       ""                   
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "fxsomme"                    
#define AIO_KEY         "aio_wtAC40eckgU3FRwGdl9A7UU8pTkA"  
#define PIN_BUTTON 2
#define PIN_EVEIL GPIO_NUM_2
#define uS_TO_mS_FACTOR 1000ULL
#define TIME_LIGHT_SLEEP  15000

char Choix_music(float temp, float humide);
void IRAM_ATTR Button_Interrupt();
void lectureCapteur();
void ConnectionWifi();
void Envoi_info_wifi(int i);
void MQTT_connect();
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish music = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/music"); // Setup a feed called 'music' for publishing.
Adafruit_BME680 bme;
const char     Titre[4][17] = {"Et quand le vent", "Il fait chaud !", "Le ciel est bleu", "Liberer, Delivre"};
const char      S_Key[4][3] = {"A4",               "B5",              "C6",                "D7"};
RTC_DATA_ATTR char Score[4] = {0,                  0,                 0,                   0};
char key; //indice de la musique jouée
bool etat = false;
short delai_button = 0;

void setup() {

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(PIN_BUTTON, Button_Interrupt, RISING);
  Serial.begin(115200);   
  Dabble.begin("Bumba Weather");
  ConnectionWifi();
  while(!bme.begin()) {
     delay(200);
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();

 }

void loop() {
  lectureCapteur();
  key = Choix_music(bme.temperature, bme.humidity);
  Dabble.begin("Bumba Weather");
  Dabble.processInput();
  Music.play(S_Key[key]);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Play :");
  lcd.setCursor(0, 1);
  lcd.print(Titre[key]);
  esp_sleep_enable_timer_wakeup(TIME_LIGHT_SLEEP * uS_TO_mS_FACTOR);      
  esp_light_sleep_start();
}

char Choix_music(float temp, float humide)
 {
     char key_c = 0;
     if (temp > 30)
     {
        key_c = 1;
     }else{
        if (temp > 15)
        {
          key_c = 2;
        }else{
          key_c = 3;
        }
     }
     if (humide > 62) key_c = 0;
     
     return key_c;
 }

 void lectureCapteur()
 {
   if (! bme.performReading()) {
   }  
 }


//Fonction d'interruption du bouton
void IRAM_ATTR Button_Interrupt()
{
  if(delai_button + 2000 < millis())  //désactivation bouton pour 2 sec (cause tremblement bouton)
  {
      etat = not etat;
      Serial.println(etat);
  }
  delai_button = millis();
}


void ConnectionWifi()
{
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }WiFi.localIP();
}


void Envoi_info_wifi(int i)
{
  MQTT_connect();
  if (! music.publish(Score[i])) {
  } else {
  }
}


void MQTT_connect() 
{
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       mqtt.disconnect();
  }
}

 
