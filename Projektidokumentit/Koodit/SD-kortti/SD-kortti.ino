/*
  Ohjelma jolla voi lukea ja kirjoittaa dataaa Sd-kortilta näytölle

   SD kortti on yhdistetty SPI väylään seuraaviin pinneihin:
 ** MOSI - pin 11 (UNO) 51 (MEGA)
 ** MISO - pin 12 (UNO) 50 (MEGA)
 ** CLK - pin 13 (UNO) 52 (MEGA) 
 ** CS - pin 10 (UNO) 53 (MEGA)
*/

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;


#define BLACK   0x0000
#define WHITE   0xFFFF

File myFile;

const int chipSelect = 4;

 const int inPin = 30; // pinni painonapille
 int val = 0; // muuttujan oin tilan lukemista varten

void setup() {
              
        Serial.begin(9600);
        uint16_t ID = tft.readID();
        Serial.print("ID loydetty = 0x");
        Serial.println(ID, HEX);
        tft.begin(ID);
        tft.setRotation(1); 
      //näytön suunnan määritys, 0 = pysty, 1 = vaaka, 2 = ylösalaisin, 3 = vaaka toisin päin
      while (!Serial) {
    }
        //Näytön värien ja tekstin paikan ja koon määritykset
        tft.fillScreen(BLACK);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(40, 20);


        Serial.print("Initializing SD card...");
        pinMode(SS, OUTPUT);

      if (!SD.begin()) {
        Serial.println("initialization failed!");
      while (1);
    }
        Serial.println("initialization done.");

      // avaa tiedoston. Ohjelmalla voi avata vain yhden tiedoston kerralla.
      myFile = SD.open("testi6.txt", FILE_WRITE);

  
      // jos tiedosto aukeaa oikein ohjelma kirjoittaa tämän:
      if (myFile) {
          Serial.print("Writing to testi6.txt...");
          myFile.println("Jouni Perkele");
          // tiedosto sulkeutuu
          myFile.close();
          Serial.println("done.");
    } 
  
      // jos tiedosto ei aukea ohjelma tulostaa error
      else {    
          Serial.println("error opening testi6.txt");
   }

      // avaa tiedoston uudestaa lukemista varten
      myFile = SD.open("testi6.txt");
      if (myFile) {
          Serial.println("testi6.txt:");


      while (myFile.available()) { 
          tft.write(myFile.read());
    }
           
      // ohjelma lukee tiedoston, ja kirjoittaa sen sarjamonitoriin ja tulostaa näytölle:
      while (myFile.available()) {  
          Serial.write(myFile.read());
          tft.println(myFile.read());
    }
    
      myFile.close();
    }
  
      // jos tiedosto ei aukea ohjelma tulostaa error
      else {
          Serial.println("error opening testi5.txt");
    }
         
}

void loop() {
      val = digitalRead(inPin);
      if(val == HIGH){
          myFile = SD.open("testi6.txt", FILE_WRITE);

  
      // jos tiedosto aukeaa oikein ohjelma kirjoittaa tämän:
      if (myFile) {
          Serial.print("Writing to testi6.txt...");
          myFile.println("Jouni Perkele");
          // tiedosto sulkeutuu
          myFile.close();
          Serial.println("done.");
    } 
  
      // jos tiedosto ei aukea ohjelma tulostaa error
      else {    
          Serial.println("error opening testi6.txt");
   }

      // avaa tiedoston uudestaa lukemista varten
      myFile = SD.open("testi6.txt");
      if (myFile) {
          Serial.println("testi6.txt:");


      while (myFile.available()) { 
          tft.write(myFile.read());
    }
           
      // ohjelma lukee tiedoston, ja kirjoittaa sen sarjamonitoriin ja tulostaa näytölle:
      while (myFile.available()) {  
          Serial.write(myFile.read());
          tft.println(myFile.read());
    }
    
      myFile.close();
    }
  
      // jos tiedosto ei aukea ohjelma tulostaa error
      else {
          Serial.println("error opening testi5.txt");
    }
         
    }
  }
    
      // setupin jälkeen ei tapahdu mitään
