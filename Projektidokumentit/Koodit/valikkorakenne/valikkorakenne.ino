#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#include <Fonts/Org_01.h>

bool backsensed = false;
bool resetsensed = false;

const int XP=7, XM=A1, YP=A2, YM=6; //240x320 ID=0x6809
const int TS_LEFT=91, TS_RT=904, TS_TOP=122, TS_BOT=922; //näytön kalibrointi

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //(data,data,data,data,sensitivity);

#define MINPRESSURE 200
#define MAXPRESSURE 1000


#define DRAW_LOOP_INTERVAL 50 //The interval used instead of delay();
int currentpage;

#include <Fonts/FreeSans9pt7b.h> //fontit
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>

#include <FreeDefaultFonts.h>

#define BLACK       0x0000      // Värit
#define NAVY        0x000F      
#define DARKGREEN   0x03E0      
#define DARKCYAN    0x03EF      
#define MAROON      0x7800      
#define PURPLE      0x780F      
#define OLIVE       0x7BE0      
#define LIGHTGREY   0xC618      
#define DARKGREY    0x7BEF      
#define BLUE        0x001F      
#define GREEN       0x07E0      
#define CYAN        0x07FF      
#define RED         0xF800      
#define MAGENTA     0xF81F      
#define YELLOW      0xFFE0      
#define WHITE       0xFFFF      
#define ORANGE      0xFDA0      
#define GREENYELLOW 0xB7E0      
#define PINK        0xFC9F


void setup()
{
  tft.reset();
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  Serial.println("Esim. Font_simple");
  Serial.print("ID loydetty = 0x");
  Serial.println(ID, HEX);
  tft.begin(ID);
  tft.setRotation(1); //näytön suunnan määritys, 0 = 0 astetta, 1 = 90 astetta 2 = 180 astetta, 3 = 270 astetta
  
  currentpage = 0;
  
  tft.fillScreen(BLACK); //määritetään minkä värinen näytön tausta on
  drawHome();  
}

void loop()
{
  TSPoint p = ts.getPoint();     // Read touchscreen
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (currentpage == 0)
  {
    if (p.z > 10 && p.z < 1000)
    {
      if (p.x > 303 && p.x < 425 && p.y > 180 && p.y < 750) //jos klikataan Measure celsius
      {
        Serial.println("Measure celsius");

        currentpage = 1;

        tft.fillRoundRect(30, 45, 240, 40, 8, WHITE);
        delay(100);

        tft.fillRoundRect(30, 45, 240, 40, 8, RED);
        tft.drawRoundRect(30, 45, 240, 40, 8, WHITE); //Measure celsius
        
        tft.setCursor(38, 57);
        tft.println("Measure celsius");
        delay(100);

        startCelsius();
      }
      if (p.x > 440 && p.x < 563 && p.y > 180 && p.y < 800) //jos klikataan Measure fahrenheit
      {
        Serial.println("Measure fahrenheit");

        currentpage = 2;
        tft.fillRoundRect(30, 95, 240, 40, 8, WHITE);
        delay(100);

        tft.fillRoundRect(30, 95, 240, 40, 8, RED);
        tft.drawRoundRect(30, 95, 240, 40, 8, WHITE); //Measure fahrenheit
        
        tft.setCursor(38, 107);
        tft.print("Measure fahrenheit");
        delay(100);

        startFahrenheit();
      }
      if (p.x > 603 && p.x < 703 && p.y > 180 && p.y < 750) //jos klikataan Calendar
      {
        Serial.println("Calendar");

        currentpage = 3;
        tft.fillRoundRect(30, 145, 240, 40, 8, WHITE);
        delay(100);

        tft.fillRoundRect(30, 145, 240, 40, 8, RED);   
        tft.drawRoundRect(30, 145, 240, 40, 8, WHITE); //Calendar
        
        tft.setCursor(38, 157);
        tft.print("Calendar");
        delay(100);

        startCalendar();
      }
      if (p.x > 733 && p.x < 903 && p.y > 180 && p.y < 750) //jos klikataan Send to phone
      {
        Serial.println("Send to phone");

        currentpage = 3;
        tft.fillRoundRect(30, 195, 240, 40, 8, WHITE);
        delay(100);

        tft.fillRoundRect(30, 195, 240, 40, 8, RED);   
        tft.drawRoundRect(30, 195, 240, 40, 8, WHITE); //Send to phone
        
        tft.setCursor(38, 207);
        tft.print("Send to phone");
        delay(100);

        startSend();
      }
  }
  }
  if (currentpage == 1)
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
      {
        backMenu();
        drawHome();
      }
  }
  if (currentpage == 2)
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
      {
        backMenu();
        drawHome();
      }
  if (currentpage == 3)
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
      {
        backMenu();
        drawHome();
      }
  if (currentpage == 4)
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    if (p.x > 743 && p.x < 850 && p.y> 105 && p.y < 200) //Takaisin nuolen klikkauskohta
      {
        backMenu();
        drawHome();
      }
}

void drawHome()
{

  tft.fillScreen(BLACK);
  tft.drawRoundRect(3, 0, 315, 240, 8, WHITE);     //Page border

  tft.fillRoundRect(30, 45, 240, 40, 8, RED);
  tft.drawRoundRect(30, 45, 240, 40, 8, WHITE); //Measure celsius
  
  tft.fillRoundRect(30, 95, 240, 40, 8, RED);
  tft.drawRoundRect(30, 95, 240, 40, 8, WHITE); //Measure fahrenheit

  tft.fillRoundRect(30, 145, 240, 40, 8, RED);   
  tft.drawRoundRect(30, 145, 240, 40, 8, WHITE); //Calendar
  
  tft.fillRoundRect(30, 195, 240, 40, 8, RED);
  tft.drawRoundRect(30, 195, 240, 40, 8, WHITE); // Send to phone


  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("MENU");

  tft.setTextColor(BLACK);
  tft.setCursor(38, 57);
  tft.print("Measure celsius");

  tft.setTextColor(BLACK);
  tft.setCursor(38, 107);
  tft.print("Measure fahrenheit");
  
  tft.setTextColor(BLACK);
  tft.setCursor(38, 157);
  tft.print("Calendar");

  tft.setTextColor(BLACK);
  tft.setCursor(38, 207);
  tft.print("Send to phone");

}
void startCelsius()
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Measure Celsius");

  tft.fillRect(30, 50, 240, 130, RED);
  tft.drawRect(30, 50, 240, 130, WHITE);
  tft.setCursor(38, 102);
  tft.setTextColor(BLACK);
  tft.print("0 astetta");
    
  backMenuIcon();
}

void startFahrenheit()
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Measure Fahrenheit");
  
  tft.fillRect(30, 50, 240, 130, RED);
  tft.drawRect(30, 50, 240, 130, WHITE);
  tft.setCursor(38, 102);
  tft.setTextColor(BLACK);
  tft.print("0 astetta");

  backMenuIcon();
}

void startCalendar()
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Calendar");
  
  backMenuIcon();
}

void startSend()
{
  tft.fillScreen(BLACK);
  tft.setCursor(32, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Send to phone");
  tft.fillRoundRect(30, 100, 240, 40, 8, YELLOW);
  tft.drawRoundRect(30, 100, 240, 40, 8, WHITE);
  tft.setCursor(38, 112);
  tft.setTextColor(BLACK);
  tft.print("Press to send");
  
  backMenuIcon();
}

void backMenu() //Takaisin napin toiminto
{
  tft.fillRoundRect(5, 195, 50, 30, 8, WHITE);
  delay(70);
  tft.fillRoundRect(5, 195, 50, 30, 8, BLUE);
  tft.drawRoundRect(5, 195, 50, 30, 8, WHITE);
  tft.setCursor(15, 204);
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
/*void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.drawFastHLine(0, y, tft.width(), BLACK); //viiva tekstien välissä
    tft.setFont(f); //Fontti mitä käytetään
    tft.setCursor(x, y); //Tekstin kohta näytössä
    tft.setTextColor(WHITE); //Tekstin väri
    tft.setTextSize(sz); //Tekstin koko
    tft.print(msg);
}
*/
