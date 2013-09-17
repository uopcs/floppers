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
  int notes[20]; // array loop will be stored in
  int data[4]; // pin data
};

struct drive drives[3];

void setupDrives(){
  for (int i = 0; i < noOfDrives; i++){
    for (int j = 0; j < 4; j++){
      drives[i].data[j] = packedData[i][j];
    }
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
    
    // a char has been pressed!
    if (c != NULL){
      expire = 170;
      currentChar = c;
    }
  }
      
  if (currentChar == 'a'){
    frame(drives[currentDrive].data, 2);
  }    
  if (currentChar == 's'){
    frame(drives[currentDrive].data, 3);
  }    
  if (currentChar == 'd'){
    frame(drives[currentDrive].data, 4);
  }    
  if (currentChar == 'f'){
    frame(drives[currentDrive].data, 5);
  }    
  if (currentChar == 'g'){
    frame(drives[currentDrive].data, 6);
  }    
  if (currentChar == 'h'){
    frame(drives[currentDrive].data, 7);
  }    
  if (currentChar == 'j'){
    frame(drives[currentDrive].data, 8);
  }    
  if (currentChar == 'k'){
    frame(drives[currentDrive].data, 9);
  }    
  if (currentChar == 'l'){
    frame(drives[currentDrive].data, 10);
  }
  
  
  

    
  
  delay(1); //  delay for step motor
}
