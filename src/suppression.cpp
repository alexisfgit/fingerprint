//programme permettant de supprimer des empreintes

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>


SoftwareSerial mySerial(2, 3); //utilisation liaison série
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); //Instancie le capteur avec un flux pour Serial


void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\nProgramme de suppression d'empreintes");

  //défini la vitesse du port série du capteur
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Capteur d'empreinte trouvé"); //Mot de passe trouvé
  } else {
    Serial.println("Capteur d'empreinte non trouvé"); //Mot de passe non trouvé
    while (1) { delay(1); }
  }
}


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available()); //Permet d'obtenir le nombre d'octets (caractères) disponibles pour la lecture sur le port série. Il s'agit des données déjà arrivées et stockées dans le tampon de réception série (qui contient 64 octets).
    num = Serial.parseInt(); //lit les caractères reçus sur le port série jusqu'à ce qu'elle atteigne un caractère qui n'est pas un chiffre
  }
  return num;
}


uint8_t deleteFingerprint(uint8_t id) {

  uint8_t p = -1;

  p = finger.deleteModel(id); //Demande au capteur d'effacer un modèle en mémoire

  if (p == FINGERPRINT_OK) {
    Serial.println("Empreinte supprimée avec succès !");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erreur de communication");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Impossible d'effectuer une suppression à cet endroit");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erreur d'écriture dans la mémoire flash");
  } else {
    Serial.print("Erreur inconnue : 0x"); Serial.println(p, HEX);
  }

  return p;
}


void loop()
{
  Serial.println("Veuillez saisir le numéro d'identification (de 1 à 127) de l'empreinte à supprimer...");
  uint8_t id = readnumber(); //id prend la valeur saisie dans le flux serial

  if (id == 0) { //1 à 127, 0 n'est pas accepté, choisir un nouvel ID
     return;
  }

  Serial.print("Suppression de l'empreinte n° ");
  Serial.println(id);

  deleteFingerprint(id); //supprime l'empreinte de la base de donnée en appelant la fonction
}