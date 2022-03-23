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
int menuPosition = 0; //the position of the scroller menu

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
    
    Serial.println(channelArray[x].minVal);
    Serial.println(channelArray[x].maxVal);
    Serial.println(channelArray[x].value);
    if (channelArray[x].minVal > channelArray[x].maxVal) {
      continue;
    }
    if (channelArray[x].value > channelArray[x].maxVal) {
      biggerMax = 1;
    }
    if (channelArray[x].value < channelArray[x].minVal) {
      smallerMin = 1;
    }
    Serial.println(smallerMin);
    Serial.println(biggerMax);
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
  lcd.setCursor(0,0);
  lcd.print("^");
  lcd.setCursor(0,1);
  lcd.print("v");
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
     //A placeholder for whatever is inputted so it doesnt give an error
    Serial.println("");
    Serial.println("BASIC");
    lcd.setBacklight(7);
    state = MAIN;
   break;

   
   case MAIN: {
    lcd.setCursor(0,0);
    lcd.print("^");
    lcd.setCursor(0,1);
    lcd.print("v");
    updateColour();
    if (lcd.readButtons() == 1) {
      
        state = SELECT;
        break;
      }
      
      
    
    
    updateColour();

    
    
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
        if (message.length() < 3) {
          Serial.println("ERROR: " + message); 
        } else {
          channelArray[elementIndex].letter = message.charAt(1);
          channelArray[elementIndex].value = 0;
          channelArray[elementIndex].minVal = 0;
          channelArray[elementIndex].maxVal = 255;
          elementIndex++;
          sortLetters();
          updateDisplay();
          
        }
        break;
     
        
      }
      case 'V': {
        if ((message.length() - 1) < 3) {
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
        if ((message.length() - 1) < 3) {
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
      if ((message.length() - 1) < 3) {
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
        Serial.println("ERROR: " + message);
      }
    break;
      
      }
      
      
    }
    break;
   }
   case SELECT: {
     while (lcd.readButtons() == 1) {
      int t = millis();
      if (t > 1000) {
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.setBacklight(5);
        lcd.print("F124497");
        
      }
       delay(1000);
     }
       lcd.clear();
       updateDisplay();
       updateColour();
       state = MAIN;
    
     
     break;
        
    }
    }
    
  }
  

      
      
  
