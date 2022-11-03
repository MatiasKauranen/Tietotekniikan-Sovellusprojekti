/*
tft.fillRoundRect(); piirtää näytölle kiinteän neliön, jossa on pyöreät kulmat.
tft.drawRoundRect(); piirtää näytölle ääriviivat neliölle, jossa on pyöreät kulmat. 
tft.setCursor(); määrittää kohdan, mihin teksti sijoittuu näytössä. 
tft.print(); tulostaa halutun asian näytölle.
tft.setTextSize(); määrittää tekstin koon.
tft.setTextColor(); määrittää tekstin värin.

*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>
#include <Fonts/Org_01.h>
#include <FreeDefaultFonts.h>
#include <SoftwareSerial.h>

const int XP=7, XM=A1, YP=A2, YM=6; //240x320 ID=0x6809
const int TS_LEFT=91, TS_RT=904, TS_TOP=122, TS_BOT=922; //näytön kalibrointi

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //(data,data,data,data,sensitivity);

#define MINPRESSURE 200 //minimi määrä, paljonko painetta täytyy antaa kosketukselle
#define MAXPRESSURE 1000 //maksimi määrä, paljonko painetta täytyy antaa kosketukselle

int currentpage;
const int inPin = 30; //Pinni painonapille :)
int val = 0; //muuttuja pin tilan lukemista varten

#define BLACK       0x0000      // Värit      
#define BLUE        0x001F      
#define GREEN       0x07E0           
#define RED         0xF800           
#define YELLOW      0xFFE0      
#define WHITE       0xFFFF      

#include "RTClib.h"   // Ensin määritetään komponentit
RTC_PCF8523 rtc; 

#include <Adafruit_MLX90614.h>    // MLX90614-sensorin määritys alkaa
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
File myFile;

const int chipSelect = 4;
void setup()
{
  Serial.begin(9600); //arduinon sarjamonitoria varten
  Serial1.begin(9600); //bluetooth kommunikaatiota varten
  while (!Serial);
  if (!mlx.begin())
  {
    Serial.println("Yhdistys MLX sensoriin ei onnistunut. Tarkista kytkentä savun halvettya.");
    while (1);
  } 
  Serial.println("================================================");   // MLX90614-sensorin määritys loppuu
  
  if (!rtc.begin())    // RTC-kellon määritys alkaa
  {
    Serial.println("RTC-kello hukassa, tarkista kytkentä!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));   // Päivämäärän ja kellonajan määritys, jos RTC:stä on katkennut virta, tai patteri on irrotettu (muisti hävinnyt)
                                                      // Päivämäärä ja kellonaika asetetaan muodossa (2021,12,22, 14,53,32)
    Serial.flush();
    while (1) delay(10);
  }
                 
  if (! rtc.initialized() || rtc.lostPower()) 
  {
    Serial.println("RTC ei ole asennettu, asetetaan seuraavaksi aika ja päivämäärä!");
  }
  
  rtc.start();  // Tähän asti päästyä RTC on asentunut ja toimii oikein, joten voidaan aloittaa varsinainen ohjelma
                  
  float drift = 43; // Kalibroi sekunnit 
  float period_sec = (7 * 86400);  // (86400 = sekunnit per päivä 1:  7 päivää = (7 * 86400) sekuntia )
  float deviation_ppm = (drift / period_sec * 1000000); //  Sekunnin poikkeama miljoonasosina
  float drift_unit = 4.34; // OFFSET-toiminto PCF8523_TwoHours-käytössä
  int offset = round(deviation_ppm / drift_unit);
  
  Serial.print("Offset is "); Serial.println(offset); // Offsetin arvo
  
  // RTC-kellon määritys loppuu
  Serial.print("Initializing SD card...");
        pinMode(SS, OUTPUT);

  if (!SD.begin()) {
        Serial.println("initialization failed!");
      while (1);
    }
  Serial.println("initialization done.");
  pinMode(inPin, INPUT); // painonappi on inputtina
  tft.reset();
  uint16_t ID = tft.readID();
  Serial.print("TFT nayton prosessori = 0x");
  Serial.println(ID, HEX);
  tft.begin(ID);
  tft.setRotation(1); //näytön suunnan määritys, 0 = 0 astetta, 1 = 90 astetta 2 = 180 astetta, 3 = 270 astetta
  
  currentpage = 0;
  pinMode(SS, OUTPUT); //Pinni 53
  pinMode(32, OUTPUT); //Laser
  tft.fillScreen(BLACK); //määritetään minkä värinen näytön tausta on
  drawHome();  
}

void loop()
{
  TSPoint p = ts.getPoint();     // Read touchscreen

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  DateTime now = rtc.now();
  val = digitalRead(inPin);
  if (val == HIGH)
    {
      saveSD();
      delay(1000);
    }
  if (currentpage == 0)
  {
    if (p.z > 10 && p.z < 1000)
    {
      if (p.x > 303 && p.x < 425 && p.y > 180 && p.y < 750) //jos klikataan Measure celsius
      {
        Serial.println("Measure celsius");

        currentpage = 1;

        tft.fillRoundRect(30, 45, 240, 40, 8, WHITE); //Välhdysanimaatio
        delay(100);

        tft.fillRoundRect(30, 45, 240, 40, 8, RED);
        tft.drawRoundRect(30, 45, 240, 40, 8, WHITE);
        
        tft.setCursor(38, 57);
        tft.println("Measure celsius");
        delay(100);

        startCelsius();
      }
      if (p.x > 440 && p.x < 563 && p.y > 180 && p.y < 800) //jos klikataan Measure fahrenheit
      {
        Serial.println("Measure fahrenheit");

        currentpage = 2;
        tft.fillRoundRect(30, 95, 240, 40, 8, WHITE); //Välhdysanimaatio
        delay(100);

        tft.fillRoundRect(30, 95, 240, 40, 8, RED);
        tft.drawRoundRect(30, 95, 240, 40, 8, WHITE);
        
        tft.setCursor(38, 107);
        tft.print("Measure fahrenheit");
        delay(100);

        startFahrenheit();
      }
      if (p.x > 603 && p.x < 703 && p.y > 180 && p.y < 750) //jos klikataan SD-Card
      {
        Serial.println("SD-Card");

        currentpage = 3;
        tft.fillRoundRect(30, 145, 240, 40, 8, WHITE); //Välhdysanimaatio
        delay(100);

        tft.fillRoundRect(30, 145, 240, 40, 8, RED);   
        tft.drawRoundRect(30, 145, 240, 40, 8, WHITE);
        
        tft.setCursor(38, 157);
        tft.print("SD-Card");
        delay(100);

        startSD();
      }
    }
  }
  if (currentpage == 1) //celsius
  { 
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 580 && p.y < 800)
      {
        measureCelsius();
        digitalWrite(32, HIGH);
        sendPhoneC();
      }
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
       {
        digitalWrite(32, LOW);
         backMenu();
         drawHome();
       }
  }
  if (currentpage == 2) //fahrenheit
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 580 && p.y < 800)
      {
        measureFahrenheit();
        digitalWrite(32, HIGH);
        sendPhoneF();
      }
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
      {
        digitalWrite(32, LOW);
        backMenu();
        drawHome();
      }
  }
  if (currentpage == 3) //SD-Card
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
      {
        backMenu();
        drawHome();
      }
  }
}

void drawHome()
{

  DateTime now = rtc.now();
  
  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 0, 315, 240, 8, WHITE);     //Valkoiset reunat näytössä

  tft.fillRoundRect(30, 45, 240, 40, 8, RED);
  tft.drawRoundRect(30, 45, 240, 40, 8, WHITE); //Measure celsius
  
  tft.fillRoundRect(30, 95, 240, 40, 8, RED);
  tft.drawRoundRect(30, 95, 240, 40, 8, WHITE); //Measure fahrenheit

  tft.fillRoundRect(30, 145, 240, 40, 8, RED);   
  tft.drawRoundRect(30, 145, 240, 40, 8, WHITE); //SD-Card

  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("MENU");

  tft.setCursor(200, 20);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.print(now.hour(), DEC); 
  tft.print(":");
  tft.print(now.minute(), DEC);
  
  tft.setCursor(200, 30);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.print(now.day() , DEC);
  tft.print(".");
  tft.print(now.month(), DEC);
  tft.print(".");
  tft.print(now.year(), DEC);

  tft.setTextColor(BLACK);
  tft.setCursor(38, 57);
  tft.setTextSize(2);
  tft.print("Measure celsius");

  tft.setTextColor(BLACK);
  tft.setCursor(38, 107);
  tft.setTextSize(2);
  tft.print("Measure fahrenheit");
  
  tft.setTextColor(BLACK);
  tft.setCursor(38, 157);
  tft.setTextSize(2);
  tft.print("SD-Card");
}
void startCelsius() //Celsius alueen näkymä
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Temperature in C");

  tft.fillRect(30, 60, 240, 70, RED);
  tft.drawRect(30, 60, 240, 70, WHITE);

  tft.fillRoundRect(200, 195, 80, 30, 8, RED);
  tft.drawRoundRect(200, 195, 80, 30, 8, WHITE);
  tft.setCursor(208, 204);
  tft.setTextColor(BLACK);
  tft.print("Press");
  
  tft.setCursor(38, 72);
  tft.setTextColor(BLACK);
  tft.print("Environment:");
    
  tft.setCursor(185, 72);
  tft.setTextColor(BLACK, RED);
  tft.print(mlx.readAmbientTempC());

  tft.setCursor(250, 72);
  tft.setTextColor(BLACK);
  tft.print("C");
  
  tft.setCursor(38, 92);
  tft.setTextColor(BLACK);
  tft.print("Target:");
  
  tft.setCursor(125, 92);
  tft.setTextColor(BLACK, RED);
  tft.print(mlx.readObjectTempC());
  
  tft.setCursor(190, 92);
  tft.setTextColor(BLACK);
  tft.print("C");

  backMenuIcon();
}
void startFahrenheit() //Fahrenheit alueen näkymä
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Temperature in F");
  
  tft.fillRect(30, 60, 240, 70, RED);
  tft.drawRect(30, 60, 240, 70, WHITE);

  tft.fillRoundRect(200, 195, 80, 30, 8, RED);
  tft.drawRoundRect(200, 195, 80, 30, 8, WHITE);
  tft.setCursor(208, 204);
  tft.setTextColor(BLACK);
  tft.print("Press");
  
  tft.setCursor(38, 72);
  tft.setTextColor(BLACK);
  tft.print("Environment:");
  
  tft.setCursor(185, 72);
  tft.setTextColor(BLACK);
  tft.print(mlx.readAmbientTempF());
  
  tft.setCursor(255, 72);
  tft.setTextColor(BLACK);
  tft.print("F");
  
  tft.setCursor(38, 92);
  tft.setTextColor(BLACK);
  tft.print("Target:");
  
  tft.setCursor(125, 92);
  tft.setTextColor(BLACK);
  tft.print(mlx.readObjectTempF());
  
  tft.setCursor(200, 92);
  tft.setTextColor(BLACK);
  tft.print("F");
  
   
  backMenuIcon();
}
void startSD() //SD-Card alueen näkymä
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("SD-Card\n");
  
  myFile = SD.open("tulokset.txt");
      if (myFile) {
          tft.println("tulokset.txt:");


      while (myFile.available()) {
        tft.write(myFile.read()); 
    }
           
      // ohjelma lukee tiedoston, ja kirjoittaa sen sarjamonitoriin
      while (myFile.available()) {
        tft.write(myFile.read());
    }
    myFile.close();
    }
  backMenuIcon();
}
void backMenu() //Takaisin napin "välähdys"
{
  tft.fillRoundRect(5, 195, 50, 30, 8, WHITE);
  delay(70);
  tft.fillRoundRect(5, 195, 50, 30, 8, BLUE);
  tft.drawRoundRect(5, 195, 50, 30, 8, WHITE);
  tft.setCursor(15, 204);
  tft.setTextColor(BLACK, BLUE);
  tft.print("<-");
  delay(70);
  tft.fillRoundRect(5, 195, 50, 30, 8, BLACK);
  currentpage = 0;
}
void backMenuIcon() //Takaisin napin kuva
{
  tft.fillRoundRect(5, 195, 50, 30, 8, BLUE);
  tft.drawRoundRect(5, 195, 50, 30, 8, WHITE);
  tft.setCursor(15, 204);
  tft.setTextColor(BLACK);
  tft.print("<-");
}
void measureCelsius() //Näyttää mitatun lämpötilan celsius asteina
{
  Serial.print("Temperature: \n"); //Nämä tulostavat mitatut lämpötilat sarjamonitoriin
  Serial.print("Environment = ");
  Serial.print(mlx.readAmbientTempC());
  Serial.print(" \xC2\xB0");
  Serial.print("C\tTarget = ");
  Serial.print(mlx.readObjectTempC());
  Serial.print(" \xC2\xB0");
  Serial.println("C");
  
  tft.setCursor(185, 72);
  tft.setTextColor(BLACK, RED);
  tft.print(mlx.readAmbientTempC());
  tft.setCursor(125, 92);
  tft.setTextColor(BLACK, RED);
  tft.setCursor(125, 92);
  tft.setTextColor(BLACK, RED);
  tft.print(mlx.readObjectTempC());
  if(mlx.readObjectTempC() > -20){ //Tietty taajuus kuuluu tietyn asteen kohdalla
    tone(24, 155, 300);
    }
  if(mlx.readObjectTempC() > -10){
    tone(24, 173, 300);
    } 
  if(mlx.readObjectTempC() > 0){
    tone(24, 196, 300);
    } 
  if(mlx.readObjectTempC() > 10){
    tone(24, 220, 300);
    } 
  if(mlx.readObjectTempC() > 20){
    tone(24, 247, 300);
    }
  if(mlx.readObjectTempC() > 30){
    tone(24, 262, 300);
    }
  if(mlx.readObjectTempC() > 40){
    tone(24, 294, 300);
    }
  if(mlx.readObjectTempC() > 50){
    tone(24, 330, 300);
    }
  if(mlx.readObjectTempC() > 60){
    tone(24, 349, 300);
    }
  if(mlx.readObjectTempC() > 70){
    tone(24, 392, 300);
    }
  if(mlx.readObjectTempC() > 80){
    tone(24, 440, 300);
    }
  if(mlx.readObjectTempC() > 90){
    tone(24, 494, 300);
    }
  if(mlx.readObjectTempC() > 100){
    tone(24, 523, 300);
    }
}
void measureFahrenheit() //Näyttää mitatun lämpötilan Fahrenheit asteina
{
  Serial.print("Temperature: \n"); //Nämä tulostavat mitatut lämpötilat sarjamonitoriin
  Serial.print("Environment = ");
  Serial.print(mlx.readAmbientTempF());
  Serial.print(" \xC2\xB0");
  Serial.print("F\tTarget = ");
  Serial.print(mlx.readObjectTempF());
  Serial.print(" \xC2\xB0");
  Serial.println("F");
  
  tft.setCursor(185, 72);
  tft.setTextColor(BLACK, RED);
  tft.print(mlx.readAmbientTempF());
  tft.setCursor(125, 92);
  tft.setTextColor(BLACK, RED);
  tft.setCursor(125, 92);
  tft.setTextColor(BLACK, RED);
  tft.print(mlx.readObjectTempF());
  if(mlx.readObjectTempC() > -20){ //Tietty taajuus kuuluu tietyn asteen kohdalla
    tone(24, 155, 300);
    }
  if(mlx.readObjectTempC() > -10){
    tone(24, 173, 300);
    } 
  if(mlx.readObjectTempC() > 0){
    tone(24, 196, 300);
    } 
  if(mlx.readObjectTempC() > 10){
    tone(24, 220, 300);
    } 
  if(mlx.readObjectTempC() > 20){
    tone(24, 247, 300);
    }
  if(mlx.readObjectTempC() > 30){
    tone(24, 262, 300);
    }
  if(mlx.readObjectTempC() > 40){
    tone(24, 294, 300);
    }
  if(mlx.readObjectTempC() > 50){
    tone(24, 330, 300);
    }
  if(mlx.readObjectTempC() > 60){
    tone(24, 349, 300);
    }
  if(mlx.readObjectTempC() > 70){
    tone(24, 392, 300);
    }
  if(mlx.readObjectTempC() > 80){
    tone(24, 440, 300);
    }
  if(mlx.readObjectTempC() > 90){
    tone(24, 494, 300);
    }
  if(mlx.readObjectTempC() > 100){
    tone(24, 523, 300);
    }
}
void saveSD()
{
  myFile = SD.open("tulokset.txt", FILE_WRITE);

  
  // jos tiedosto aukeaa oikein ohjelma kirjoittaa tämän:
    if (myFile) {
      DateTime now = rtc.now();
      Serial.print("Writing to tulokset.txt...");
      myFile.print(now.hour(), DEC); 
      myFile.print(":");
      myFile.print(now.minute(), DEC);
      myFile.print(":");
      myFile.print(now.second(), DEC);
      myFile.print(", ");
      myFile.print(now.day() , DEC);
      myFile.print(".");
      myFile.print(now.month(), DEC);
      myFile.print(".");
      myFile.println(now.year(), DEC);
       
      myFile.println("Temperature:"); //Nämä tulostavat mitatut lämpötilat sarjamonitoriin
      myFile.print("Environment = ");
      myFile.print(mlx.readAmbientTempC());
      myFile.print(" C\nTarget = ");
      myFile.print(mlx.readObjectTempC());
      myFile.println(" C\n");
      
      myFile.println("Temperature:"); //Nämä tulostavat mitatut lämpötilat sarjamonitoriin
      myFile.print("Environment = ");
      myFile.print(mlx.readAmbientTempF());
      myFile.print(" F\nTarget = ");
      myFile.print(mlx.readObjectTempF());
      myFile.println(" F\n");
      // tiedosto sulkeutuu
      myFile.close();
      Serial.println("done.");
  } 
  
  // jos tiedosto ei aukea ohjelma tulostaa error
    else {    
      Serial.println("error opening tulokset.txt");
  }
  myFile = SD.open("tulokset.txt");
      if (myFile) {
          Serial.println("tulokset.txt:");


      while (myFile.available()) {
        Serial.write(myFile.read()); 
    }
           
      // ohjelma lukee tiedoston, ja kirjoittaa sen sarjamonitoriin
      while (myFile.available()) {  
        Serial.write(myFile.read());
    }
    myFile.close();
      }
}
void sendPhoneC()
{
  DateTime now = rtc.now();
  Serial1.print(now.hour(), DEC); 
  Serial1.print(":");
  Serial1.print(now.minute(), DEC);
  Serial1.print(":");
  Serial1.print(now.second(), DEC);
  Serial1.print(", ");
  Serial1.print(now.day() , DEC);
  Serial1.print(".");
  Serial1.print(now.month(), DEC);
  Serial1.print(".");
  Serial1.println(now.year(), DEC);
  Serial1.print("Temperature: \n"); //Nämä tulostavat mitatut lämpötilat puhelinsovellukseen
  Serial1.print("Environment = ");
  Serial1.print(mlx.readAmbientTempC());
  Serial1.print(" \xC2\xB0");
  Serial1.print("C\tTarget = ");
  Serial1.print(mlx.readObjectTempC());
  Serial1.print(" \xC2\xB0");
  Serial1.println("C");
}
void sendPhoneF()
{
  DateTime now = rtc.now();
  Serial1.print(now.hour(), DEC); 
  Serial1.print(":");
  Serial1.print(now.minute(), DEC);
  Serial1.print(":");
  Serial1.print(now.second(), DEC);
  Serial1.print(", ");
  Serial1.print(now.day() , DEC);
  Serial1.print(".");
  Serial1.print(now.month(), DEC);
  Serial1.print(".");
  Serial1.println(now.year(), DEC);
  Serial1.print("Temperature: \n"); //Nämä tulostavat mitatut lämpötilat puhelinsovellukseen
  Serial1.print("Environment = ");
  Serial1.print(mlx.readAmbientTempF());
  Serial1.print(" \xC2\xB0");
  Serial1.print("F\tTarget = ");
  Serial1.print(mlx.readObjectTempF());
  Serial1.print(" \xC2\xB0");
  Serial1.println("F");
}
