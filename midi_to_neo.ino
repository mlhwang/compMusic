#include <Adafruit_NeoMatrix.h>
#include <gamma.h>

#include <MIDIUSB.h>

#define LED_RING 3
#define LED_PIN 13
#define NUM 24

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, LED_RING, NEO_GRB + NEO_KHZ800);

int led[NUM][3];
int notes[NUM];

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  pixels.begin();
  
  for (int i = 0; i < NUM; i++) {
    for (int j = 0; j < 3; j++) {
       led[i][j]=0;
    }
  }
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    MidiUSB.sendMIDI(rx);
    if (rx.header == 9) {
      digitalWrite(LED_PIN,1);
      int note=rx.byte2;
      int vol=rx.byte3;
      int group=note/NUM; 
      int number=note%NUM; 
      
      notes[number]+=3*vol+128;
      notes[number]=max(notes[number],4096);
      int r=0,g=0,b=0;
      switch(group){
        case 0: 
          r=16;g=0;b=0; break;
        case 1: 
          r=12;g=6;b=0; break;
        case 2: 
          r=0;g=16;b=0; break;
        case 3: 
          r=0;g=6;b=12; break;
        case 4: 
          r=0;g=0;b=16; break;
        default: 
          r=6;g=6;b=6;
      }
      r*=vol;g*=vol;b*=vol;
      led[number][1]=max(led[number][1],r);
      led[number][2]=max(led[number][2],g);
      led[number][3]=max(led[number][3],b);
      
      digitalWrite(LED_PIN,0);
    }
    
    if (rx.header == 8) {
      digitalWrite(LED_PIN,1);
      int note=rx.byte2;
      int number=note%NUM; 
      
      notes[number]=max(notes[number]-512,0);
      
      digitalWrite(LED_PIN,0);
    }
  } while (rx.header != 0);
  
  for (int i = 0; i < NUM; i++) {
    pixels.setPixelColor(NUM-i-1, pixels.Color(led[i][1]/16,led[i][2]/16,led[i][3]/16) ) ; 
    
    led[i][1]-=max(led[i][1]/(24+notes[i]/64),notes[i]?0:5);
    led[i][2]-=max(led[i][2]/(24+notes[i]/64),notes[i]?0:5);
    led[i][3]-=max(led[i][3]/(24+notes[i]/64),notes[i]?0:5);
    led[i][1]=max(led[i][1],0);
    led[i][2]=max(led[i][2],0);
    led[i][3]=max(led[i][3],0);

    notes[i]=max(notes[i]-64,0);
  }
  pixels.show();
  delay(2);
}
