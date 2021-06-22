/*
  Genus - Memory Game

Author: Rodrigo Rodovalho
Date: 06/22/2021

*/


#include <LiquidCrystal.h>

#define DIFICULTY_EASY 3;
#define DIFICULTY_MEDIUM 2;
#define DIFICULTY_HARD 1;

#define HAS_LCD true

#define IS_DEBUG fals

//buzzer freq
const int FREQ_A = 261; //Do
const int FREQ_B = 293; //Re
const int FREQ_C = 329; //Mi
const int FREQ_GAME_OVER = 400;

//pin
const int PIN_BUZZER = 9;
const int PIN_LED_A = 10;
const int PIN_LED_B = 11;
const int PIN_LED_C = 12;
const int PIN_BUTTON_A = 6;
const int PIN_BUTTON_B = 7;
const int PIN_BUTTON_C = 8;
const int PIN_BUTTON_CONTROL = 13;

int level = 0;
int levelPointer = 1;
int dificulty = DIFICULTY_HARD;
unsigned int signalSource[50];

unsigned int i;

bool isWaitingResponse = false;
bool isWaitingGameStart = true;

unsigned int highScore = 0;

#if HAS_LCD
  const int RS = 5, EN = 4, D4 = 3, D5 = 2, D6 = 1, D7 = 0;
  LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
#endif

void setup() {

  #if HAS_LCD
    lcd.begin(16, 2); // 16 colunas e 2 linhas
    lcd.clear();
  #endif

  #if IS_DEBUG
    Serial.begin(9600);
  #endif
  
  // put your setup code here, to run once:
  pinMode(PIN_LED_A, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LED_C, OUTPUT);
  
  pinMode(PIN_BUZZER, OUTPUT); 

  pinMode(PIN_BUTTON_A, INPUT_PULLUP); 
  pinMode(PIN_BUTTON_B, INPUT_PULLUP); 
  pinMode(PIN_BUTTON_C, INPUT_PULLUP); 
  pinMode(PIN_BUTTON_CONTROL, INPUT_PULLUP); 

  resetGame();

}

void showLightA(){

  #if IS_DEBUG
    Serial.println("showLightA");
  #endif
  
  digitalWrite(PIN_LED_A, HIGH);
  digitalWrite(PIN_LED_B, LOW);
  digitalWrite(PIN_LED_C, LOW);
  tone(PIN_BUZZER, FREQ_A);
  delay(200 * dificulty);
  noTone(PIN_BUZZER);
}

void showLightB(){

  #if IS_DEBUG
    Serial.println("showLightB");
  #endif
  
  digitalWrite(PIN_LED_A, LOW);
  digitalWrite(PIN_LED_B, HIGH);
  digitalWrite(PIN_LED_C, LOW);
  tone(PIN_BUZZER, FREQ_B);  
  delay(200 * dificulty);
  noTone(PIN_BUZZER);
}

void showLightC(){

  #if IS_DEBUG
    Serial.println("showLightC");
  #endif
    
  digitalWrite(PIN_LED_A, LOW);
  digitalWrite(PIN_LED_B, LOW);
  digitalWrite(PIN_LED_C, HIGH);
  tone(PIN_BUZZER, FREQ_C);
  delay(200 * dificulty);
  noTone(PIN_BUZZER);
}

void watchInput(){

  #if IS_DEBUG
    Serial.println("Starting watch input");
  #endif
  
  bool pushed_A = digitalRead (PIN_BUTTON_A) == LOW;
  bool pushed_B = digitalRead (PIN_BUTTON_B) == LOW;
  bool pushed_C = digitalRead (PIN_BUTTON_C) == LOW;

  if (pushed_A){ //button A pressed
    showLightA();
    checkResponse(1);
  } else if (pushed_B){ //button B pressed
    showLightB();
    checkResponse(2);
  } else if(pushed_C) { //button C pressed
    showLightC();
    checkResponse(3);
  }
}

void printLcd(int collumn, int row, String value){
  #if HAS_LCD
    lcd.setCursor(collumn, row); 
    lcd.print(value);
  #else
    Serial.println(value);
  #endif  
  
}

void printLcd(int collumn, int row, int value){
  #if HAS_LCD
    lcd.setCursor(collumn, row); 
    lcd.print(value);
  #else
    Serial.println(value);
  #endif  
}

void checkResponse(int inputSignal){
    #if IS_DEBUG
      Serial.print("checkResponse: ");
      Serial.println(inputSignal);
    #endif

  if (signalSource[levelPointer] == inputSignal){ //right answer
    if (level == levelPointer){       
      level++;  
      printLcd(6,0,level+1);
      printLcd(0,1,"Correct         ");
      delay(500);
      printLcd(0,1,"                ");
      signalSource[level] = random(1,4);
      levelPointer = 0;
      isWaitingResponse = false; 

      #if IS_DEBUG
        Serial.print("Level Up: ");
        Serial.println(inputSignal);
      #endif
      
    } else {
      levelPointer++; 
    }
  } else {
    if (level > highScore){
      #if IS_DEBUG
        Serial.print("New HighScore");
        Serial.println(level);
      #endif
      highScore = level;
    }
    gameOver();
  }
  turnLightsOff();
  
}

void gameOver(){
  #if IS_DEBUG
     Serial.print("Game Over ");
     Serial.println(level);
  #endif
  printLcd(0,1,"Game Over     ");
  tone(PIN_BUZZER, FREQ_GAME_OVER);
  digitalWrite(PIN_LED_A, HIGH);
  digitalWrite(PIN_LED_B, HIGH);
  digitalWrite(PIN_LED_C, HIGH);
  delay(2000);
  noTone(PIN_BUZZER);
  resetGame();
}

void turnLightsOff(){
  digitalWrite(PIN_LED_A, LOW);
  digitalWrite(PIN_LED_B, LOW);
  digitalWrite(PIN_LED_C, LOW);
}

void resetGame(){
  level = 0;
  levelPointer = 0;
  signalSource[level] = random(1,4);
 
  isWaitingResponse = false;

  #if HAS_LCD
    lcd.clear();
  #endif

  printLcd(0,0,"*Genus* Score:");
  printLcd(14,0,highScore);
  printLcd(0,1,"Press to start");
  
  isWaitingGameStart = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(isWaitingGameStart){
    #if IS_DEBUG 
      Serial.println("Waiting game start");
    #endif
    bool pushed_D = digitalRead (PIN_BUTTON_CONTROL) == LOW;
    if (pushed_D){
      printLcd(0,0, "Level 1         ");
      printLcd(0,1, "                ");
      isWaitingGameStart = false;
      delay(300);  
    } else {
      delay(500);   
    }
  } else {
    delay(300);
    if (!isWaitingResponse){
      turnLightsOff();
      #if IS_DEBUG
        Serial.println("Start sequence");
      #endif
      
      for (i = 0; i < level + 1; i++) {
          #if IS_DEBUG
            Serial.println(signalSource[i]);
          #endif
          
          switch (signalSource[i]){
            case 1:
              showLightA();
              break;
            case 2:
              showLightB();
              break;
            case 3:
              showLightC();
              break;
          }
         turnLightsOff();
         delay(500); 
      }
      isWaitingResponse = true;
    }  
    watchInput();
  }
}
