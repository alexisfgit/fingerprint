//programme permettant d'enregistrer des empreintes

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>


SoftwareSerial mySerial(2, 3); //utilisation liaison série sur l'Arduino
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); //Instancie le capteur avec un flux pour Serial
uint8_t id;


void setup()
{
  Serial.begin(9600); //Instancie la liaison série
  Serial.println("\n\nProgramme d'ajout d'empreintes'");

  //défini la vitesse du port série du capteur
  finger.begin(57600);

  //Bloc permettant de vérifier si le capteur est actif et s'il répond
  if (finger.verifyPassword()) { //Vérifie le mot de passe d'accès au capteur, retourne vrai si le mot de passe est valide
    Serial.println("Capteur d'empreinte trouvé");
  } else {
    Serial.println("Capteur d'empreinte non trouvé");
    while (1) { delay(1); }
  }

  /*Serial.println(F("Reading sensor parameters"));                             |
  finger.getParameters();                                                       |
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);        |
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);         |
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);               |Permet de récupérer les paramètres du capteur et de les afficher
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);   |
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX); |
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);           |
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);             |*/
}


//la fonction attend que des données soient disponibles sur le port série, puis lit un nombre entier depuis ce port dès qu'il est disponible, et le retourne. La fonction ne retourne que lorsque le nombre lu est différent de zéro.
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available()); //Permet d'obtenir le nombre d'octets (caractères) disponibles pour la lecture sur le port série. Il s'agit des données déjà arrivées et stockées dans le tampon de réception série (qui contient 64 octets).
    num = Serial.parseInt(); //lit les caractères reçus sur le port série jusqu'à ce qu'elle atteigne un caractère qui n'est pas un chiffre
  }
  return num;
}


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("En attente d'un doigt valide pour s'inscrire en tant que "); Serial.println(id);

  while (p != FINGERPRINT_OK) { //tant que le retour de p est différent de FINGERPRINT_OK, redemander la prise d'empreinte
    p = finger.getImage(); //Demande au capteur de prendre une image du doigt appuyé sur la surface et de stocker la réponse dans la variable p
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image du doigt prise avec succès");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication, merci de réessayer");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Ereur lors de la prise d'image, merci de réessayer");
      break;
    default:
      Serial.println("Erreur inconnue, merci de réessayer");
      break;
    }
  }
  //Succès


  p = finger.image2Tz(1); //Demande au capteur de convertir l'image en modèle (un modèle = une empreinte) | (1)pour la première prise d'empreinte

  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image convertie avec succès");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image trop brouillée");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Impossible de trouver les caractéristiques des empreintes digitales");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Impossible de trouver les caractéristiques des empreintes digitales");
      return p;
    default:
      Serial.println("Erreur inconnue");
      return p;
  }

  Serial.println("Retirez votre doigt du capteur");
  delay(2000); //Attente de 2000ms
  p = 0; //initialisation de p sur 0

  while (p != FINGERPRINT_NOFINGER) { //tant que le retour de p est différent de FINGERPRINT_NOFINGER, reste dans la boucle
    p = finger.getImage(); //Vérifie que le doigt n'est pas posé sur le capteur. Retour attendu : FINGERPRINT_NOFINGER
  }

  Serial.print("Empreinte n° "); Serial.println(id);
  p = -1;
  Serial.println("Replacer le même doigt sur le capteur");

  while (p != FINGERPRINT_OK) { //tant que le retour de p est différent de FINGERPRINT_OK, redemander la prise d'empreinte
    p = finger.getImage(); //prend une image de l'empreinte (2ème fois sur la même empreinte)
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image du doigt prise avec succès");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication, merci de réessayer");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Ereur lors de la prise d'image, merci de réessayer");
      break;
    default:
      Serial.println("Erreur inconnue, merci de réessayer");
      break;
    }
  }
  //Succès


  p = finger.image2Tz(2); //Demande au capteur de convertir l'image en modèle (un modèle = une empreinte) | (2)pour vérification de l'empreinte

  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image convertie avec succès");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image trop brouillée");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Impossible de trouver les caractéristiques des empreintes digitales");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Impossible de trouver les caractéristiques des empreintes digitales");
      return p;
    default:
      Serial.println("Erreur inconnue");
      return p;
  }
  //Image convertie


  Serial.print("Création du modèle pour l'empreinte : ");  Serial.println(id);
  p = finger.createModel(); //Demande au capteur de prendre les deux modèles pour n'en créer qu'un.

  if (p == FINGERPRINT_OK) {
    Serial.println("Impressions assorties !");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erreur de communication");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Les empreintes ne sont pas identiques");
    return p;
  } else {
    Serial.println("Erreur inconnue");
    return p;
  }

  Serial.print("Empreinte n° : "); Serial.println(id);
  p = finger.storeModel(id); //enregistre le modèle calculé en vue d'une mise en correspondance ultérieure
  
  if (p == FINGERPRINT_OK) {
    Serial.println("Image de l'empreinte enregistrée avec succès");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erreur de communication");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Impossible de stocker l'empreinte dans cet emplacement");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erreur d'écriture dans la mémoire flash");
    return p;
  } else {
    Serial.println("Erreur inconnue");
    return p;
  }

  return true;
}


void loop()
{
  Serial.println("Prêt à enregistrer une empreinte");
  Serial.println("Veuillez saisir le numéro d'identification (de 1 à 127) sous lequel vous souhaitez enregistrer ce doigt...");
  id = readnumber(); //id prend la valeur saisie dans le flux serial
  if (id == 0) { //1 à 127, 0 n'est pas accepté, choisir un nouvel ID
     return;
  }
  Serial.print("Numéro d'identification de l'empreinte: ");
  Serial.println(id);

  while (! getFingerprintEnroll() ); //Appelle la fonction de manière répétée jusqu'à ce qu'elle retourne une valeur vraie
}