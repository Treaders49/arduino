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
struct channels{
  char letter;
  int value;
  int minVal;
  int maxVal;
  String description;
  };

struct channels channelArray[26];

int elementIndex = 0;
int menuPosition = 0; //the position of the scroller menu

void sortLetters() {
  for (int x = 0 ; x <=  elementIndex; x++) {
    if (elementIndex == 1) {
      break;
    }
    for (int y = 0 ; y < elementIndex - 1; y++) {
      if (channelArray[y].letter > channelArray[y+1].letter) {
        
          char temp, temp1, temp2, temp3, temp4  = channelArray[y].letter, channelArray[y].value, channelArray[y].minVal, channelArray[y].letter, channelArray[y].description ; //swapping the values
          temp1 = channelArray[y].value;
          temp2 = channelArray[y].minVal;
          channelArray[y].letter, channelArray[y].value, channelArray[y].minVal, channelArray[y].letter, channelArray[y].description = channelArray[y+1].letter, channelArray[y+1].value, channelArray[y+1].minVal, channelArray[y+1].letter, channelArray[y+1].description ;
          channelArray[y+1].letter, channelArray[y+1].value, channelArray[y+1].minVal, channelArray[y+1].letter, channelArray[y+1].description  = temp, temp1, temp2, temp3, temp4;
        
        
        
      }
      
    } 
  }
  
}

int getIndex(int value) {
  int index = 0;
  for (index; index < 26; index++) {
    if (channelArray[index].letter == value) {
      return index;
      
    }
    
  }
  return -1; //denotes letter not found
}
void updateDisplay() {
  lcd.setCursor(1,0);
  lcd.print(channelArray[menuPosition].letter);
  lcd.setCursor(2,0);
  Serial.println();
  Serial.println(channelArray[menuPosition].value);
  if (channelArray[menuPosition].value < 100) {
      lcd.print("0"); 
      lcd.setCursor(3,0);
      if (channelArray[menuPosition].value < 10) {
          lcd.print("0"); 
          lcd.setCursor(4,0);
        }
      
    }
  lcd.print(channelArray[menuPosition].value);
  
  if (elementIndex > 1) {
    lcd.setCursor(1,1);
    lcd.print(channelArray[menuPosition+1].letter);
    lcd.setCursor(2,1);
    if (channelArray[menuPosition+1].value < 100) {
        lcd.print("0"); 
        lcd.setCursor(3,1);
        if (channelArray[menuPosition+1].value < 10) {
          lcd.print("0"); 
          lcd.setCursor(4,1);
        }
      }
     lcd.print(channelArray[menuPosition+1].value);
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
        updateDisplay();
    }
    if ((lcd.readButtons() == 8) && (menuPosition >=1)) {
        menuPosition = menuPosition - 1;
        updateDisplay();
    }
    String message = Serial.readString(); 
    switch(message.charAt(0)) {
      case 'C': {
        channelArray[elementIndex].letter = message.charAt(1);
        channelArray[elementIndex].value = 0;
        channelArray[elementIndex].minVal = 0;
        channelArray[elementIndex].maxVal = 255;
        elementIndex++;
        sortLetters();
        updateDisplay();
        break;
     
        
      }
      case 'V': {
        int index = getIndex(message.charAt(1));
        if (index == -1) {
          Serial.println("channel selected does not exist");
        } else {
          
          channelArray[index].value = (message.substring(2,5)).toInt();
          
        }
        updateDisplay();
        break;
      }
      case 'N': {
        
      }
    }
   }
  }
      
      
  

}
