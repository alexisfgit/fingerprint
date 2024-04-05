
//programme permettant la vérification des empreintes en effectuant une effectuant avec la base de données. Il faut au préalable enregistrer ses empreintes via le programme "ajout.cpp"

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <U8x8lib.h> //choix de cette librairie car prend peu de RAM sur le Uno

const int relay = 10;

SoftwareSerial mySerial(2, 3); //utilisation liaison série sur l'Arduino
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); //Instancie le capteur avec un flux pour Serial
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ A5, /* data=*/ A4, /* reset=*/ U8X8_PIN_NONE); //OLED sans broche reset


void setup()
{
  pinMode(relay, OUTPUT);
  u8x8.begin(); //Instancie l'affichage sur l'écran
  u8x8.setFont(u8x8_font_pressstart2p_f); //police d'écriture. _f : prend en charge + de caractères
  delay(100); //temps d'initialisation de l'écran

  /*Serial.begin(9600); //Instancie la liaison série
  Serial.println("\n\nProgramme de détection du doigt");*/

  u8x8.drawString(2,2,"Programme de");
  u8x8.drawUTF8(2,3, "détection du");
  u8x8.drawString(5,4,"doigt");

  //défini la vitesse du port série du capteur
  finger.begin(57600);

  u8x8.clearDisplay();

  //Bloc permettant de vérifier si le capteur est actif et s'il répond
  if (finger.verifyPassword()) { //Vérifie le mot de passe d'accès au capteur, retourne vrai si le mot de passe est valide
    //Serial.println("Capteur d'empreinte trouvé");
    u8x8.drawString(5,2,"Capteur");
    u8x8.drawString(3,3,"d'empreinte");
    u8x8.drawUTF8(5,4,"trouvé");
  } else {
    //Serial.println("Capteur d'empreinte non trouvé");
    u8x8.drawString(5,2,"Capteur");
    u8x8.drawString(3,3,"d'empreinte");
    u8x8.drawUTF8(4,4,"non trouvé");
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

  finger.getTemplateCount(); //Demande au capteur le nombre de modèles (empreintes) stockés en mémoire. Le nombre est stocké dans templateCount en cas de succès.

  /*S'il n'y a pas de modèle dans la base de données, retourne un message invitant l'utilisateur à en enregistrer via le programme "ajout"
  Sinon l'utilisateur peut aposer son doigt sur le capteur*/
  if (finger.templateCount == 0) {
    //Serial.print("Le capteur ne contient pas de modèle d'empreintes, ouvrir le programme ajout pour en ajouter");
    delay(1500);
    u8x8.clearDisplay();
    u8x8.drawString(3,2,"Le capteur");
    u8x8.drawString(3,3,"ne contient");
    u8x8.drawUTF8(1,4,"pas de modèles");
    u8x8.drawString(2,5,"d'empreintes");
  }
  else {
    /*Serial.print("Le capteur contient "); Serial.print(finger.templateCount); Serial.println(" modèles d'empreintes");
    Serial.println("En attente d'un doigt valide...");*/
    delay(1500);
    u8x8.clearDisplay();

    u8x8.drawString(3,2,"Le capteur");
    u8x8.drawString(4,3,"contient");
    u8x8.setCursor(3,4);
    u8x8.print(finger.templateCount);
    u8x8.drawUTF8(6,4,"modèles");
    u8x8.drawString(2,5,"d'empreintes");
    
    delay(1500);
    u8x8.clearDisplay();

    u8x8.drawString(3,2,"Posez votre");
    u8x8.drawString(4,3,"doigt sur");
    u8x8.drawString(3,4,"le capteur");
    u8x8.drawString(4,5,"........");
  }
}


//Fonction permettant la prise d'empreinte et de la comparer à la base de donnée du capteur
uint8_t getFingerprintID() {
  digitalWrite(relay, LOW); //fermeture serrure
  uint8_t p = finger.getImage(); //Demande au capteur de prendre une image du doigt appuyé sur la surface et de stocker la réponse dans la variable p
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image du doigt prise avec succès");
      u8x8.clearDisplay();

      u8x8.drawString(1,2,"Image du doigt");
      u8x8.drawString(3,3,"prise avec"); 
      u8x8.drawUTF8(5,4,"succès");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.print(".");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Erreur de communication, merci de réessayer");
      u8x8.clearDisplay();

      u8x8.drawString(3,2,"Erreur de");
      u8x8.drawString(2,3,"communication."); 
      u8x8.drawUTF8(3,4,"Réessayez...");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println("Ereur lors de la prise d'image, merci de nettoyer le capteur et de réessayer");
      u8x8.clearDisplay();

      u8x8.drawString(2,2,"Erreur prise");
      u8x8.drawString(3,3,"d'image."); 
      u8x8.drawUTF8(3,4,"Réessayez...");
      return p;
    default:
      //Serial.println("Erreur inconnue, merci de nettoyer le capteur et de réessayer");
      u8x8.clearDisplay();

      u8x8.drawString(1,3,"Erreur inconnue"); 
      u8x8.drawUTF8(3,4,"Réessayez...");
      return p;
  }

  //Si image de l'empreinte OK, convertir cette image en un modèle
  p = finger.image2Tz(); //Demande au capteur de convertir l'image en modèle (un modèle = une empreinte)
  /*switch (p) {
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
  }*/

  //Si la conversion de l'image en modèle OK, rechercher dans la base de données des modèles, celui correspondant
  p = finger.fingerSearch(); //Demande au capteur de rechercher les caractéristiques de l'empreinte prise précédemment et de trouver une correspondance avec un modèle de la base de données. L'emplacement de la correspondance est stocké dans fingerID (défini dans le programme "ajout" et le degré de confiance de la correspondance dans confidence
  if (p == FINGERPRINT_OK) {
    //Serial.println("Correspondance trouvée !");
    u8x8.clearDisplay();

    u8x8.drawString(1,3,"Correspondance");
    u8x8.drawUTF8(3,4,"trouvée !");
    digitalWrite(relay, HIGH); //ouverture serrure
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Erreur de communication");
    u8x8.clearDisplay();

    u8x8.drawString(3,2,"Erreur de");
    u8x8.drawString(2,3,"communication.");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    //Serial.println("Correspondance non trouvée, nettoyer le capteur ou essayer un autre doigt");
    u8x8.clearDisplay();

    u8x8.drawString(1,3,"Correspondance");
    u8x8.drawUTF8(2,4,"non trouvée");
    return p;
  } else {
    //Serial.println("Erreur inconnue");
    u8x8.clearDisplay();

    u8x8.drawString(1,3,"Erreur inconnue");
    return p;
  }

  //Si une correspondance est trouvée, affiche le numéro de l'empreinte et son degré de confiance
  /*Serial.print("Empreinte n° "); Serial.print(finger.fingerID); Serial.print(" trouvée");
  Serial.print(" avec un degré de confiance de "); Serial.println(finger.confidence);*/
  delay(1500);
  u8x8.clearDisplay();

  u8x8.drawUTF8(1,1,"Empreinte n° ");
  u8x8.setCursor(14,1);
  u8x8.print(finger.fingerID);
  u8x8.drawUTF8(3,2,"trouvée !");
  u8x8.drawUTF8(3,4,"Degré de");
  u8x8.drawString(2,5,"confiance de");
  u8x8.setCursor(5,6);
  u8x8.print(finger.confidence);

  delay(2000);
  u8x8.clearDisplay();

  u8x8.drawString(3,2,"Posez votre");
  u8x8.drawString(4,3,"doigt sur");
  u8x8.drawString(3,4,"le capteur");
  u8x8.drawString(4,5,"........");

  return finger.fingerID; //Retourne le numéro de l'empreinte trouvée
}


void loop()
{
  getFingerprintID(); //appelle la fonction de vérification d'empreinte
  delay(50);
}