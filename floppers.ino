// code for musical floppies!

#include <PS2Keyboard.h>
#include<stdio.h>

// 7 segment pins
int f = 13;
int e = 12;
int d = A0;
int g = A2;
int a = A3;
int b = A4;
int c = A5;
int segments[7] = {a, b, c, d, e, f, g};

const int DataPin = 3;
const int IRQpin =  2;

int packedData[3][4] = {{4, 5, 1, 0}, {6, 7, 1, 0}, {8, 9, 1, 0}}; // floppy drive array
int currentDrive = 0;
int counter = 0;
int noOfDrives = 3;


typedef struct drive drive; // struct for drives
struct drive{
  int notes[200]; // array loop will be stored in
  // notes are 60ms long, meaning max loop is 12 seconds
  int data[4]; // pin data
  int currentNote;
  boolean recording;
  boolean looping;
  int loopCap; // length of the loop of notes, defaults as 200
};

struct drive drives[3];

void setupDrives(){
  for (int i = 0; i < noOfDrives; i++){
    for (int j = 0; j < 4; j++){
      drives[i].data[j] = packedData[i][j];
    }
    resetDrive(i);
  }
}

char currentChar = NULL; // null = no music
int expire = 0;

PS2Keyboard keyboard;

void tick(int pin){ // tick step motor
  digitalWrite(pin, 1);
  digitalWrite(pin, 0);
}

void multiplex(int pin){
  // for seven segment
  digitalWrite(pin, 1);
  digitalWrite(pin, 0);
}

void writeNum(int num){
  num++;
  if (num == 1){
    multiplex(b);
    multiplex(c);
  } else if (num == 2){
    multiplex(a);
    multiplex(b);
    multiplex(g);
    multiplex(e);
    multiplex(d);
  } else if (num == 3){
    multiplex(b);
    multiplex(c);
    multiplex(a);
    multiplex(g);
    multiplex(d);
  }
  
  for (int i = 0; i < 7; i++){
    pinMode(segments[i], OUTPUT);
    digitalWrite(segments[i], 1);
  }
}

void resetDrive(int id){
    drives[id].currentNote = 0;
    drives[id].recording = false;
    drives[id].looping = false;
    drives[id].loopCap = 200; // default 
    for (int i = 0; i < 200; i++){
      drives[id].notes[i] = 0; // reset recording
    }
}

void frame(int drive[], float freq){
  if (freq == 0){
    return;
  }
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
  for (int i = 0; i < 7; i++){
    pinMode(segments[i], OUTPUT);
    digitalWrite(segments[i], 1);
  }
}



void loop() {
  for (int i = 0; i < 3; i++){ // make sure it's fairly bright
    writeNum(currentDrive);
  }
  
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
      resetDrive(currentDrive); // the current drive cannot be looping
    }
    
    if (c == PS2_DELETE){
      currentDrive = 0;
      for (int i = 0; i < noOfDrives; i++){
        resetDrive(i);
      }
    }
    
    if (c == PS2_TAB) {
      drives[currentDrive].recording = true;
      drives[currentDrive].loopCap = 200; // default
      
    }
    
    if (c == PS2_ESC) {
      // end loop
      if (drives[currentDrive].recording == true){
        drives[currentDrive].loopCap = drives[currentDrive].currentNote;
      }
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
  
  
  // recording
  if (drives[currentDrive].recording){
    if (counter % 60 == 0){ // read every 120ms
      //Serial.print(freq);
      drives[currentDrive].notes[drives[currentDrive].currentNote] = freq;
      drives[currentDrive].currentNote++;
      if (drives[currentDrive].currentNote >= drives[currentDrive].loopCap){
        drives[currentDrive].recording = false; // turn recording off
        drives[currentDrive].currentNote = 0; // reset currentNote
        drives[currentDrive].looping = true;
        //Serial.println();
        currentDrive++; // increment drive id
        if (currentDrive >= noOfDrives){
          currentDrive = 0;
        }
        resetDrive(currentDrive); // the current drive cannot be looping
      }
    }
  }
  
  // looping
  for (int i = 0; i < noOfDrives; i++){
    if (drives[i].looping){
      // this drive is in loop cycle
      int driveFreq = drives[i].notes[drives[i].currentNote];
      //Serial.println(driveFreq);
      frame(drives[i].data, driveFreq);
      if (counter % 60 == 0){ // change note every 120ms
        drives[i].currentNote++;
        if (drives[i].currentNote >= drives[i].loopCap){
          drives[i].currentNote = 0; // restart loop!
        }
      }
    }
  }
  
  
  


  
   delay(1); // 1ms delay for step motor
  
  

}
