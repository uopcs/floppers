// code for musical floppies!

#include <PS2Keyboard.h>
#include<stdio.h>

const int DataPin = 3;
const int IRQpin =  2;

int packedData[3][4] = {{4, 5, 1, 0}, {6, 7, 1, 0}, {8, 9, 1, 0}}; // floppy drive array
int currentDrive = 0;
int counter = 0;
int noOfDrives = 3;


typedef struct drive drive; // struct for drives
struct drive{
  int notes[50]; // array loop will be stored in
  int data[4]; // pin data
  int currentNote;
  boolean recording;
  boolean looping;
};

struct drive drives[3];

void setupDrives(){
  for (int i = 0; i < noOfDrives; i++){
    for (int j = 0; j < 4; j++){
      drives[i].data[j] = packedData[i][j];
    }
    drives[i].currentNote = 0;
    drives[i].recording = false;
    drives[i].looping = false;
  }
}

char currentChar = NULL; // null = no music
int expire = 0;

PS2Keyboard keyboard;

void tick(int pin){ // tick step motor
  digitalWrite(pin, 1);
  digitalWrite(pin, 0);
}

void frame(int drive[], float freq){
  if (counter % int(freq) == 0){
    // move actuator
    tick(drive[1]);
    drive[2]++; // increment counter
    
    if (drive[2] % 90 == 0){
      // change direction of actuator
      if (drive[3] == 0){
        drive[3] = 1;
      } else {
        drive[3] = 0;
      }
      digitalWrite(drive[0], drive[3]);
    }
  } 
}

void setup() {
  delay(1000);
  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);
  setupDrives();
  for (int i = 0; i < noOfDrives; i++){
    pinMode(drives[i].data[0], OUTPUT);
    pinMode(drives[i].data[1], OUTPUT);
  }
}



void loop() {
  
  
  counter++;
  expire--;
  if (expire < 0){
    currentChar = NULL;
  }
  if (keyboard.available()) {
    // read the next key
    char c = keyboard.read();
    
    // change current drive if enter pressed
    if (c == PS2_ENTER) {
      currentDrive++;
      if (currentDrive >= noOfDrives){
        currentDrive = 0;
      }
    }
    
    
    if (c == PS2_TAB) {
      drives[currentDrive].recording = true;
    }
    
    // a char has been pressed!
    if (c != NULL){
      expire = 120;
      currentChar = c;
    }
  }
  int freq; // get frequency of next note
  switch(currentChar){
    case 'a':
      freq = 2;
      break;
    case 's':
      freq = 3;
      break;
    case 'd':
      freq = 4;
      break;
    case 'f':
      freq = 5;
      break;
    case 'g':
      freq = 6;
      break;
    case 'h':
      freq = 7;
      break;
    case 'j':
      freq = 8;
      break;
    case 'k':
      freq = 9;
      break;
    case 'l':
      freq = 10;
      break;
    default:
    freq = 0;
  }
  if (freq != 0){
    frame(drives[currentDrive].data, freq);
  }
  
  if (drives[currentDrive].recording){
    if (counter % 120 == 0){ // read every 120ms
      Serial.print(freq);
      drives[currentDrive].currentNote++;
      if (drives[currentDrive].currentNote >= 50){
        drives[currentDrive].recording = false; // turn recording off
        drives[currentDrive].currentNote = 0; // reset currentNote
        Serial.println();
      }
    }
  }
  
  
  


  
   delay(1); // 1ms delay for step motor
  
  

}
