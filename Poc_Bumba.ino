
//-----------DEFINE--------------

//Define I2C (BME680 et LCD)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <LiquidCrystal_I2C.h>

/*//Define wifi
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
*/
//Define pour Dabble
#define CUSTOM_SETTINGS
#define INCLUDE_MUSIC_MODULE
#include <DabbleESP32.h>

/*//Define pour Adafruit
#define WLAN_SSID       "FpooiWifi"                         //nom du point d'accès téléphone
#define WLAN_PASS       ""                                  //mdp du point d'accès
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                    //use 8883 for SSL
#define AIO_USERNAME    "fxsomme"                           //username du compte Adafruit
#define AIO_KEY         "aio_wtAC40eckgU3FRwGdl9A7UU8pTkA"  //clé d'accès au compte Adafruit
*/
//Define génériques
#define PIN_BUTTON 2
#define PIN_EVEIL GPIO_NUM_2
#define uS_TO_mS_FACTOR 1000ULL
#define TIME_LIGHT_SLEEP  15000



//--------------BRANCHEMENTS---------------
// Connectez le bouton à D2
// Connect "SCL" pin on the BME680 and LCD with GPIO 22 on the ESP32
// Connect "SDA" pin on the BME680 and LCD with GPIO 21 on the ESP32



//---------DEFINITION DES FONCTIONS--------------
//Fonction pour choisir la musique
char Choix_music(float temp, float humide);

//Fonction d'iterruption du boutton
void IRAM_ATTR Button_Interrupt();

//Fonction qui lit les valeurs du capteur de température
void lectureCapteur();

//Fonction de connection au réseau wifi
void ConnectionWifi();

//Fonction d'accès à Adafruit et envoie des données
void Envoi_info_wifi(int i);

//Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect();



//-------------VARIABLES GLOBALES-----------
//Définition de la taille du LCD
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/*//Préparation Adafruit
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish music = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/music"); // Setup a feed called 'music' for publishing.
*/
//Choix du mode de communication I2C avec le BME680
Adafruit_BME680 bme;

//Tableau des musiques enregistrées
const char     Titre[4][17] = {"Et quand le vent", "Il fait chaud !", "Le ciel est bleu", "Liberer, Delivre"};
const char      S_Key[4][3] = {"A4",               "B5",              "C6",                "D7"};
RTC_DATA_ATTR char Score[4] = {0,                  0,                 0,                   0};

//Autres variables
char key; //indice de la musique jouée
bool etat = false;
short delai_button = 0;



//################### SETUP ##########################
void setup() {

  //Configuration du bouton en interruption
  pinMode(PIN_BUTTON, INPUT);
  attachInterrupt(PIN_BUTTON, Button_Interrupt, RISING);
  
  //Init UART
  Serial.begin(115200);   

  //Init BME
  while(!bme.begin()) {
   //Serial.println("Could not find a valid BME680 sensor, check wiring!");
   delay(200);
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  //bme.setPressureOversampling(BME680_OS_4X);
  //bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  //bme.setGasHeater(320, 150); // 320*C for 150 ms

  //Init LCD
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();

 }


//######################### LOOP #######################
void loop() {
  
  //lecture du capteur
  lectureCapteur();

  //choix de la music
  key = Choix_music(bme.temperature, bme.humidity);

  //connection dabble
  Dabble.begin("Bumba Weather");
  Dabble.processInput();
  //Serial.println(esp32ble.available());
  //delay(2000);
    
  //Envoie de la music à dabble
  Music.play(S_Key[key]);
  //Music.playMusic(1, S_Key[key]);
  //Music.addToQueue(S_Key[key]);

  //Envoie info wifi
  ConnectionWifi();
  Envoi_info_wifi(key);

  //Affichage LCD du titre de la music
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Play :");
  lcd.setCursor(0, 1);
  lcd.print(Titre[key]);

  //Light sleep pour 15s
  //Serial.println("..zzZ Ligh sleep start Zzz..");
  esp_sleep_enable_timer_wakeup(TIME_LIGHT_SLEEP * uS_TO_mS_FACTOR);      
  esp_light_sleep_start();
   
  /*esp_sleep_enable_timer_wakeup(50000 * uS_TO_mS_FACTOR);
  esp_sleep_enable_ext0_wakeup(PIN_BUTTON,1);
  esp_deep_sleep_start();*/

 }


//--------------- FONCTIONS -------------------------
//Choix de la musique en fonction des données de temps
//Return : indice de la musique
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
     Serial.println("Failed to perform reading :(");
   }  
   Serial.print("Temperature = ");
   Serial.print(bme.temperature);
   Serial.println(" *C");
   Serial.print("Humidity = ");
   Serial.print(bme.humidity);
   Serial.println(" %");
 }


//Fonction d'interruption du bouton
void IRAM_ATTR Button_Interrupt()
{
  if(delai_button + 2000 < millis())  //désactivation bouton pour 2 sec (a cause tremblement bouton)
  {
      etat = not etat;
      if (etat == true)
      {
        Serial.println("Réveil !");
      }else{
        Serial.println("Deep Sleep Zzz...");
      }
        if (etat == false)
      {
        delay(100);
        esp_sleep_enable_ext0_wakeup(PIN_EVEIL,1);
        esp_deep_sleep_start();
      }
  }
  delai_button = millis();

  
}


void ConnectionWifi()
{
  /*Serial.println(F("Adafruit MQTT Start"));
  // Connect to WiFi access point.
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());*/
}


void Envoi_info_wifi(int i)
{
  /*MQTT_connect();
  Serial.print(F("\nSending music title "));
  Serial.print(++(Score[i]));
  if (! music.publish(Score[i])) {
    Serial.println(F(" - Failed"));
  } else {
    Serial.println(F(" - OK!"));
  }*/
}


void MQTT_connect() 
{
  /*int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
  }
  Serial.println("MQTT Connected!");*/
}

 
