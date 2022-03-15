#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  lcd.setBacklight(5);
}
#define SYNC 0
#define MAIN 1
int state = SYNC;
String channels[26][4];
int elementIndex = 0;
int menuPosition = 0; //the position of the scroller menu

void sortLetters() {
  for (int x = 0 ; x <=  elementIndex; x++) {
    if (elementIndex == 1) {
      break;
    }
    for (int y = 0 ; y < elementIndex - 1; y++) {
      if (channels[y][0] > channels[y+1][0]) {
        String temp = channels[y][0];
        channels[y][0] = channels[y+1][0] ;
        channels[y+1][0] = temp;
        
      }
      
    } 
  }
  
}

void loop() {
  int x, y = 0;
  switch(state) {
    case SYNC: {
      while (not Serial.available()) {
        lcd.setCursor(x,y);
        Serial.print("Q");
        x++;
        delay(1000);
      }
     }
    Serial.print("BASIC");
    lcd.setBacklight(7);
    state = MAIN;
   break;

   
   case MAIN: {
    lcd.setCursor(0,0);
    lcd.print("^");
    lcd.setCursor(0,1);
    lcd.print("v");
    if ((lcd.readButtons() == 4) && (menuPosition < (elementIndex-2))) {
        menuPosition =  menuPosition + 1;
        Serial.println(menuPosition);

        lcd.setCursor(1,0);
        lcd.print(channels[menuPosition][0]);
        lcd.setCursor(1,1);
        lcd.print(channels[menuPosition+1][0]);
    }
    if ((lcd.readButtons() == 8) && (menuPosition >=1)) {
        menuPosition = menuPosition - 1;
        Serial.println(menuPosition);
        lcd.setCursor(1,0);
        lcd.print(channels[menuPosition][0]);
        lcd.setCursor(1,1);
        lcd.print(channels[menuPosition+1][0]);
    }
    String message = Serial.readString(); 
    switch(message.charAt(0)) {
      case 'C': {
        channels[elementIndex][0] = message.charAt(1);
        elementIndex++;
        sortLetters();
        lcd.setCursor(1,0);
        lcd.print(channels[menuPosition][0]);
        lcd.setCursor(1,1);
        lcd.print(channels[menuPosition+1][0]);
        
        
      }
      case 'V': {
        
      }
      case 'N': {
        
      }
    }
   }
  }
      
      
  

}
