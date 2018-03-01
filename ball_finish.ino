 #include "pitches.h"
int latchPin = 8;
int clockPin = 12;
int dataPin = 11;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int melody2[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5,  NOTE_G5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
int noteDurations2[] = {
  4, 4, 4, 4, 4, 4, 4
};
//定義共陽極七段顯示器的字型(digit pattern)
// define the LED digit patterns, from 0 - 9
// 0 = LED on, 1 = LED off, in this order:
//                Mapping to       a,b,c,d,e,f,g of Seven-Segment LED
byte matrix[8] = { B11111111,
                   B11111111,
                   B11111111,
                   B11111111,
                   B11111111,
                   B11111111,
                   B11111111,
                   B11111111, };

byte readyMatrix[8] = { B11111111,
                        B11011011,
                        B11011011,
                        B11111111,
                        B10111101,
                        B11011011,
                        B11100111,
                        B11111111, };

byte overMatrix[8] = { B11111111,
                       B11011011,
                       B11011011,
                       B11111111,
                       B11100111,
                       B11011011,
                       B10111101,
                       B11111111, };
                      
#define fsr_pin A0
// 定義每個七段顯示器顯示的時間 (延遲時間), 預設 1ms 
int delay_time = 1;
int dotX[5] = {0};
int dotY[5] = {0};
int iFlag=1,iFlag2=1;
unsigned long int dotTimer;

enum State {
  READY,
  GAME,
  OVER
};
State state = READY;
                          
void setup() {
  Serial.begin(115200);
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(dataPin,OUTPUT);

  toInit();
}
 
void loop() {
  switch (state) {
  case READY:
    toReady();
    readymusic();
    break;
  case GAME:
    game();
    break;
  case OVER:
    over();
    failmusic();
    break;
  }

  for(int i = 0; i < 8 ; i++) {
    digitalWrite(latchPin,LOW);
    pickDigit(i);//這行不能註解
    shiftOut(dataPin, clockPin, MSBFIRST, matrix[i]);
    digitalWrite(latchPin,HIGH);
    delay(delay_time);
  }
}

void toInit() {
  for (int i = 0; i < 8; i++)
  matrix[i] = B11111111;
  for (int i = 0; i < 5; i++) {
    dotX[i] = random(0, 8);
    dotY[i] = 0 - i;
  }
  matrix[dotY[0]] = transbyte(dotX[0]);
  dotTimer = millis();
}

void toReady() {
  iFlag=1;
  for (int i = 0; i < 8; i++)
  matrix[i] = readyMatrix[i];
  
  int fsr_value = analogRead(fsr_pin); // 讀取FSR
  if(fsr_value >550)fsr_value=550;//讓壓力不要報表
  int n = map(fsr_value, 0, 550, 0, 7);
  if (n > 3) {
    toInit();
    state = GAME;
  }
}

void game() {
   iFlag=1;
  int fsr_value = analogRead(fsr_pin); // 讀取FSR
  if(fsr_value >550)fsr_value=550;//讓壓力不要報表
  int dist = map(fsr_value, 0, 550, 0, 7);
  matrix[5] = transbyte(dist);
  matrix[6] = transbyte(dist);

  if ((millis() - dotTimer) > 1000 * 0.5) {
    for (int i = 0; i < 5; i++) {
      if (dotY[i] < 4) {
        dotY[i]++;
      } else {
        if (matrix[dotY[i]] == (matrix[dotY[i]] & matrix[5])) {
          state = OVER;
        }
        dotX[i] = random(0, 8);
        dotY[i] = 0;
      }
      dotTimer = millis();
    }
  }
  
  for (int i = 0; i < 5; i++) {
    Serial.println(i);
    Serial.println(dotY[i]);
    if (dotY[i] >= 0) matrix[dotY[i]] = transbyte(dotX[i]);
  }
}

void over(){
  iFlag2=1;
  for (int i = 0; i < 8; i++)
  matrix[i] = overMatrix[i];
    
  int fsr_value = analogRead(fsr_pin); // 讀取FSR
  if(fsr_value >550)fsr_value=550;//讓壓力不要報表
  int n = map(fsr_value, 0, 550, 0, 7);
  
   
  if(n>1) {
    toInit();
    state = READY;
  }

}

byte transbyte(int n) {
  switch (n) {
  case 0:
    return B01111111;
    case 1:
    return B10111111;
    case 2:
    return B11011111;
    case 3:
    return B11101111;
    case 4:
    return B11110111;
    case 5:
    return B11111011;
    case 6:
    return B11111101;
    case 7:
    return B11111110;
  }
}

void pickDigit(int x) {//一個很神奇的函式，缺他就會顯示錯誤
  byte rowByte = 1 << x;
  shiftOut(dataPin, clockPin, MSBFIRST, rowByte);
}

void failmusic(){
  while(iFlag){
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(7, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);} 
    iFlag=0;
  }
 }

 void readymusic(){
  while(iFlag2){
  for (int thisNote = 0; thisNote < 7; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations2[thisNote];
    tone(7, melody2[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);} 
    iFlag2=0;
  }
 }






