
        // Ensin määritetään komponentit
        #include "RTClib.h"
        RTC_PCF8523 rtc; 

            // MLX90614-sensorin määritys alkaa
            #include <Adafruit_MLX90614.h>
            Adafruit_MLX90614 mlx = Adafruit_MLX90614();
            void setup () {
            Serial.begin(9600);
            while (!Serial);
            if (!mlx.begin()) {
            Serial.println("Yhdistys MLX sensoriin ei onnistunut. Tarkista kytkentä savun halvettya.");
            while (1);
            }; 
            Serial.println("================================================");
            // MLX90614-sensorin määritys loppuu


                  // RTC-kellon määritys alkaa
                  if (! rtc.begin()) {
                  Serial.println("RTC-kello hukassa, tarkista kytkentä!");
                  Serial.flush();
                  while (1) delay(10);
                  }
                
                  if (! rtc.initialized() || rtc.lostPower()) {
                  Serial.println("RTC ei ole asennettu, asetetaan seuraavaksi aika ja päivämäärä!");
                  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));   // Päivämäärän ja kellonajan määritys, jos RTC:stä on katkennut virta, tai patteri on irrotettu (muisti hävinnyt)
                                                                      // Päivämäärä ja kellonaika asetetaan muodossa (2021,12,22, 14,53,32)
                  }
                  rtc.start();  // Tähän asti päästyä RTC on asentunut ja toimii oikein, joten voidaan aloittaa varsinainen ohjelma
                
                  float drift = 43; // Kalibroi sekunnit 
                  float period_sec = (7 * 86400);  // (86400 = sekunnit per päivä 1:  7 päivää = (7 * 86400) sekuntia )
                  float deviation_ppm = (drift / period_sec * 1000000); //  Sekunnin poikkeama miljoonasosina
                  float drift_unit = 4.34; // OFFSET-toiminto PCF8523_TwoHours-käytössä
                  int offset = round(deviation_ppm / drift_unit);
 

                  Serial.print("Offset is "); Serial.println(offset); // Offsetin arvo
                  }
                  // RTC-kellon määritys loppuu





        // Tästä alkaa varsinainen ohjelma
        void loop () {
        DateTime now = rtc.now();
        
        Serial.print("Paivamaara: ");
        (Serial.print(now.year() , DEC )) ;
        Serial.print("-");
        (Serial.print(now.month(), DEC )) ;
        Serial.print("-");
        (Serial.print(now.day(), DEC));
        Serial.print("\n");


        Serial.print("Kellonaika: ");
        (Serial.print(now.hour(), DEC)) ; 
        Serial.print(":");
        (Serial.print(now.minute(), DEC)) ; 
        Serial.print(":");
        (Serial.print(now.second(), DEC));
        Serial.print("\n");
        
       
        Serial.print("Lampotila: \n");
        Serial.print("Ymparisto = "); Serial.print(mlx.readAmbientTempC());
        Serial.print(" \xC2\xB0"); Serial.print("C\tKohde = ");
        Serial.print(mlx.readObjectTempC()); Serial.print(" \xC2\xB0");
        Serial.println("C");Serial.print("            ");Serial.print(mlx.readAmbientTempF());
        Serial.print(" \xC2\xB0");Serial.print("F\t        ");
        Serial.print(mlx.readObjectTempF());Serial.print(" \xC2\xB0");
        Serial.println("F");
        Serial.println();
        
        if(mlx.readObjectTempC() > -20){
        tone(9, 200, 300);
        } 
        if(mlx.readObjectTempC() > -10){
        tone(9, 200, 300);
        } 
        if(mlx.readObjectTempC() > 0){
        tone(9, 200, 300);
        } 
        if(mlx.readObjectTempC() > 10){
        tone(9, 230, 300);
        }
        if(mlx.readObjectTempC() > 25){
        tone(9, 262, 300);
        }
        if(mlx.readObjectTempC() > 26){
        tone(9, 294, 300);
        }
        if(mlx.readObjectTempC() > 27){
        tone(9, 330, 300);
        }
        if(mlx.readObjectTempC() > 28){
        tone(9, 349, 300);
        }
        if(mlx.readObjectTempC() > 60){
        tone(9, 392, 300);
        }
        if(mlx.readObjectTempC() > 70){
        tone(9, 200, 300);
        }  
 
        Serial.print("\n");
        delay(1000); }
