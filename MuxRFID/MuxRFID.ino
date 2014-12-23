
#include <SoftwareSerial.h>

#define READERNUM 9

//Mux control pins no.
int s0 = 8;
int s1 = 9;
int s2 = 10;
int s3 = 11;

// RFID reset pin no.
const int RFIDResetPin = 5;

// Tag char array (replace whatever the tag you have)
char myTag[13] = "6A008FBD336B";

// variable for input number
int whichReader_pre=9;

// speaker pin no.
const int speakerPin = 12;

// reset button pin
const int resetButtonPin = 4;
int buttonState_pre = 1;

// softwareSerial for RFID readers
SoftwareSerial rfidSerial(2, 3);

// led Mode Pin
const int ledPin = 6;
int button_reading_pre;
int isPressing= 1;

long previousMillis = 0;  
long interval = 1000;   

void setup(){
  Serial.begin(9600);
  
  rfidSerial.begin(9600);
 
  // setting the RFID reset pin
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  // setting Mux board   
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  
  // beep test
  tone(speakerPin, 880 ,8);
  pinMode(resetButtonPin,INPUT_PULLUP);
  pinMode(ledPin,OUTPUT);
  
}

void loop(){
  // check button input
  checkButton();
 
  // check RFID readers
  for(int i=0;i<READERNUM;i++){
    if(isPressing == 1) checkRFIDReader(i);
  }
 
}


void checkButton(){
   // getting inverted input 1 = off, 0 = on
   int button_reading = digitalRead(resetButtonPin);
   isPressing = button_reading;
   
   // Do something with this serial command
    if(button_reading != button_reading_pre && button_reading == 0){
     
      Serial.print('H');
      Serial.print(",");
      Serial.print(10, DEC);
      Serial.println();
      
    }
    button_reading_pre = button_reading;
}

void checkRFIDReader(int reader){
   // setting Mux board with channel number
   setMux(reader);
   // reset Reader
   resetReader(reader); 
   // can be used for delaying signal input
   // delay(0);

  // wait until getting 14 chars from RFID tags
  if(rfidSerial.available() >= 14){
    
    //proces at a 2 as it marks the tag begining
    while(rfidSerial.read() != 2);

    //temporal buffer to hold our tag
    char tagString[13];
    
    //read the serial until we gat a 3, the end of the tag
    int charactersRead = rfidSerial.readBytesUntil(3, tagString, 13);
    //Check if it is a match
    checkTag(tagString,reader);

  } 
}

void checkTag(char tag[], int whichReader){
  
  if(strlen(tag) == 0) return; //empty, no need to contunue

  if(compareTag(tag, myTag)){ // if matched myTag, do this
    // send RFID number to Serial with H header
      Serial.print('H');
      Serial.print(",");
      Serial.print(whichReader, DEC);
      Serial.println();
    
    if(whichReader_pre != whichReader){
      
      //generate sound for each input
      tone(speakerPin, 880 ,8);
    }
    whichReader_pre = whichReader;
  
  }

}

boolean compareTag(char one[], char two[]){

  if(strlen(one) == 0) return false; //empty
  
  // mismatches
  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //match
}

// resetReaders 
void resetReader(int reader){
  
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150); 
  
}



// setMux function
void setMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

}



