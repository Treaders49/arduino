#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


#ifdef __arm__ //used to find the free memory
  extern "C" char* sbrk(int incr);
#else // __ARM__
  extern char *__brkval;
#endif // __arm__
int freeMemory() {
  char top;
  #ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
  #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
  #else // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
  #endif // __arm__
  }


void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  lcd.setBacklight(5);
  
}
#define SYNC 0
#define MAIN 1
#define SELECT 2

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
int menuPosition = 0;//the position of the scroller menu
int messageNo = 0;
void sortLetters() {
  for (int x = 0 ; x <=  elementIndex; x++) {
    if (elementIndex == 1) {
      break;
    }
    for (int y = 0 ; y < elementIndex - 1; y++) {
      if (channelArray[y].letter > channelArray[y+1].letter) {
        channels temp = channelArray[y];
        channelArray[y] = channelArray[y+1];
        channelArray[y+1] =  temp;
          
        
        
      }
      
    } 
  }
  
}

void updateColour() {
  int smallerMin = 0;
  int biggerMax = 0;
  for (int x = 0; x < elementIndex; x++) {
    if (channelArray[x].minVal > channelArray[x].maxVal) {
      continue;
    }
    if (channelArray[x].value > channelArray[x].maxVal) {
      biggerMax = 1;
    }
    if (channelArray[x].value < channelArray[x].minVal) {
      smallerMin = 1;
    }
    
  }
   
   if ((smallerMin == 1) && (biggerMax == 0)) {
    lcd.setBacklight(2);
  }
  if ((biggerMax == 1) && (smallerMin == 0)) {
    lcd.setBacklight(1);
  } 
  if ((smallerMin == 1) && (biggerMax == 1)) {
    lcd.setBacklight(3);
  }
  
  
  if ((smallerMin == 0) && (biggerMax == 0)) {
    lcd.setBacklight(7);
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
  if (elementIndex > 0) {
    lcd.setCursor(1,0);
    lcd.print(channelArray[menuPosition].letter);
    lcd.setCursor(2,0);
    if (channelArray[menuPosition].value < 100) {
      lcd.print(" ");
      lcd.setCursor(3,0);
      
      if (channelArray[menuPosition].value < 10) { 
          lcd.print(" ");
          lcd.setCursor(4,0);
          
        }
      
      
    }
    lcd.print(channelArray[menuPosition].value);
    lcd.setCursor(6,0);
    lcd.print(channelArray[menuPosition].description);
  }
  
  
  
  if (elementIndex > 1) {
    lcd.setCursor(1,1);
    lcd.print(channelArray[menuPosition+1].letter);
    lcd.setCursor(2,1);
    if (channelArray[menuPosition+1].value < 100) { 
        lcd.print(" ");
        lcd.setCursor(3,1);
        
        if (channelArray[menuPosition+1].value < 10) {
          lcd.print(" ");
          lcd.setCursor(4,1);
        }
      }
     lcd.print(channelArray[menuPosition+1].value);
     lcd.setCursor(6,1);
     lcd.print(channelArray[menuPosition+1].description);
    }
    
  }
  void displayArrows() {
    lcd.clear();
    byte name0x0[] = { B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00000 };
    byte name1x0[] = { B00000, B00100, B00100, B00100, B00100, B10101, B01110, B00100 };
    lcd.begin(16, 2);
  
    lcd.createChar(0, name0x0);
    if (menuPosition > 0) {
      lcd.write(0);
    }
    
  
    lcd.createChar(1, name1x0);
    lcd.setCursor(0, 1);
    if (((elementIndex - 2) != menuPosition) && (elementIndex >= 3)) {
      lcd.write(1);
    }
    
  }
bool isDuplicate(char channelName) {
  bool duplicate = false;
  for (int x = 0 ; x <=  elementIndex; x++) {
    if (channelName == channelArray[x].letter) {
      duplicate = true;
      break;
    }
  }
  return duplicate;
}
  
  
  


void loop() {
  static long t;
  int x, y = 0;
  switch(state) {
    case SYNC: {
        Serial.print("Q");
        delay(1000);
        if (Serial.available()){
          char s = Serial.read();
          if (s == 'X') {
            Serial.println("");
            Serial.println("UDCHARS, FREERAM, NAMES");
            lcd.setBacklight(7);
            displayArrows();
            state = MAIN;
            break;
          }
        }
      
      break;
     }
     
    
  

   
   case MAIN: {
    
    updateColour();
    if (lcd.readButtons() == 1) {
        t = millis();
        state = SELECT;
        break;
      }
    
    
    if ((lcd.readButtons() == 4) && (menuPosition < (elementIndex-2))) {
        menuPosition =  menuPosition + 1;
        displayArrows();
        updateDisplay();
        updateColour();
    }
    if ((lcd.readButtons() == 8) && (menuPosition >=1)) {
        menuPosition = menuPosition - 1;
        displayArrows();
        updateDisplay();
        updateColour();
    }
    String message = Serial.readString();
    
      switch(message.charAt(0)) {
      case 'C': {
        if (( not isDuplicate(message.charAt(1)) && (isAlpha(message.charAt(1))) && (isUpperCase(message.charAt(1))))) {
          if (message.length() < 3) {
          Serial.println("ERROR: " + message); 
        } else {
          channelArray[elementIndex].letter = message.charAt(1);
          channelArray[elementIndex].description = message.substring(2, (message.length() - 1));
          channelArray[elementIndex].value = 0;
          channelArray[elementIndex].minVal = 0;
          channelArray[elementIndex].maxVal = 255;
          elementIndex++;
          sortLetters();
          displayArrows();
          updateDisplay();
          }
        } else {
          Serial.println("Error: " + message);
        }
        
        
        break;
     
        
      
      }
      case 'V': {
        if (((message.length() - 1) < 3) || ((message.substring(2,5).toInt()) > 255) || ((message.substring(2,5).toInt()) < 0)) {
      Serial.println("ERROR: " + message); 
    } else {
        int index = getIndex(message.charAt(1));
        if (index == -1) {
          Serial.println("channel selected does not exist");
        } else {
          
          channelArray[index].value = (message.substring(2,5)).toInt();
          
        }
        updateDisplay();
      }
        updateColour();
        break;
      }
      case 'N': {
        if (((message.length() - 1) < 3 || ((message.substring(2,5).toInt()) > 255) || ((message.substring(2,5).toInt()) < 0))) {
      Serial.println("ERROR: " + message); 
    } else {
        int index = getIndex(message.charAt(1));
        if (index == -1) {
          Serial.println("channel selected does not exist");
        } else {
          
          channelArray[index].minVal = (message.substring(2,5)).toInt();
          
      }
    }
     updateColour();
     break;
    }
    case 'X': {
      if (((message.length() - 1) < 3) || ((message.substring(2,5).toInt()) > 255) || ((message.substring(2,5).toInt()) < 0)) {
      Serial.println("ERROR: " + message); 
    } else {
        int index = getIndex(message.charAt(1));
        if (index == -1) {
          Serial.println("channel selected does not exist");
        } else {
          
          channelArray[index].maxVal = (message.substring(2,5)).toInt();
          
      }
    }
      updateColour();
      break;
    }
    default: { 
      if (message != NULL) {
        if (messageNo == 0) {
          messageNo += 1;
        } else {
          Serial.println("ERROR: " + message);
        }
        
      }
    break;
      
      }
      
      
    }
    break;
   }
   case SELECT: {
     while (lcd.readButtons() == 1) {
      
      long timeNow = millis();
      
      if ((timeNow - t) > 1000) {
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.setBacklight(5);
        lcd.print("F124497");
        lcd.setCursor(8,0);
        lcd.print(freeMemory());
        
      }
       delay(750);
     }
       
       lcd.clear();
       displayArrows();
       updateDisplay();
       updateColour();
       state = MAIN;
    
     
     break;
        
    }
    }
    
  }
  

      
      
  
