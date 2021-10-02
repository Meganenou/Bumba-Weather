# Bumba-Weather
### **MOORGHEN Mégane, MOORGHEN Shéréna, POURCHER Franck et SOMME François-Xavier**

_Proof of Concept - IoT et Consommation - Objets connectés à faible consommation_

Le projet que notre équipe a décidé de réaliser se nomme _Bumba Weather_. Son système permet de jouer une musique en fonction de l’ambiance environnante. Le _Bumba Weather_ permet de divertir et de relaxer un utilisateur dans un milieu intérieur comme extérieur.

## Matériels nécessaires
### Composants
* ESP32
* Écran LCD, utilisable en I2C
* SEN-BME680, capteur de température et d'humidité
* Bouton poussoir
* Résistance à 10 kohm

<h4 id="archi"> Architecture du système</h4>
<p align="center"><img src="https://user-images.githubusercontent.com/63911484/135721294-0bdc901b-79c8-41f4-928b-df5af23a6cc5.png" title="Architecture du système - Bumba Weather" width=80%></p>

### IDE et Librairies
* Installer Arduino : <a> https://www.arduino.cc/en/software </a>
* Télecharger les librairies suivantes :
  * `Adafruit BME680 Library` par _Adafruit_
  * `LiquidCrystal I2C` par _Marco Schwartz_
  * `Adafruit IO Arduino` par _Adafruit_
  * `DabbleESP32` par _Mimansa Maheshwari et Dhrupal Shah_

### Dabble
Application mobile  permettant de transformer un smartphone en un périphérique d’E/S virtuel et de contrôler le matériel via Bluetooth, de communiquer avec lui et accéder à des fonctionnalités du portable.

Dans le cadre de ce PoC, seule la fonctionnalité _Music_ est utilisée. Dans cette fonctionnalité, chaque clé correspond à une musique spécifique (par exemple : la clé **C4**). L’utilisateur peut changer pour chaque clé la musique avec celles présentes sur dans son répertoire.

## Tutoriel
* Faire les branchements nécessaires, voir <a href="https://github.com/Meganenou/Bumba-Weather/edit/main/README.md#archi">Architecture du système</a>
* Activer le Bluetooth sur le smartphone
* Sur Arduino, compiler le script suivant : https://github.com/Meganenou/Bumba-Weather/blob/main/Poc_Bumba.ino
* Sur Dabble, associez-vous à l'appareil _Bumba Weather_

***Enjoy your music ~🎶***
