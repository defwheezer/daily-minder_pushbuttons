#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Chronodot.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Chronodot RTC;
Adafruit_7segment matrix = Adafruit_7segment();

#define PIXELPIN 12  //pin controlling neopixels (remember to add 470 ohm resistor!)
#define SWITCHPIN 11  //pin with switch to GND (normally open)

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 7

const int buttonPins[] = {4,5,6,7,8,9,10,11};
bool buttonState[] = {0,0,0,0,0,0,0,0};
bool temp_buttonState[] = {0,0,0,0,0,0,0,0};
int LEDState[] = {0,0,0,0,0,0,0,0};
int prev_LEDState[] = {0,0,0,0,0,0,0,0};

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN, NEO_RGB);

int delayval = 100; // delay for half a second
int gcolor = 50; //green color intensity
int ygcolor = 25; //green color intensity
int yrcolor = 25; //red color intensity

int hours = 0;
bool daylightsavings = false;
int minutes = 0;
int seconds = 0;
int weekdays;
int dayOfMonth;
int month;

bool blinkColon = true;

int showType = 0;

void setup() {
  // Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  //Serial.println("testing from pro mini!");
  
  // This line sets the RTC with an explicit date & time, for example to set
  // April 1, 2020 at 11:35:00am you would call:
  // RTC.adjust(DateTime(2020, 4, 1, 11, 35, 0));
  
  // uncomment below to adjust time to computer
  //RTC.adjust(DateTime(__DATE__, __TIME__)); 

   matrix.begin(0x70); //7-seg matrix display
   matrix.setBrightness(5);
   
    // initialize the LED pin as an output:
    pinMode(PIXELPIN, OUTPUT);      
  
   // initialize reset button pin
   pinMode(SWITCHPIN, INPUT_PULLUP);
  
   // initialize the pushbutton pin as an input:
   for(int i =0; i<8;i++) {
     pinMode(buttonPins[i], INPUT_PULLUP); 
   }
   
  pixels.begin(); // This initializes the NeoPixel library.

  //end setup
}


void showLEDgreen(int pixelnum) {
  pixels.setPixelColor(pixelnum, pixels.Color(0,gcolor,0)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void showLEDyellow(int pixelnum) {
  pixels.setPixelColor(pixelnum, pixels.Color(yrcolor,ygcolor,0)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void hideLED(int pixelnum) {
  pixels.setPixelColor(pixelnum, pixels.Color(0,0,0)); // off.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void showLEDs(int today) {
  for(int i=0;i<NUMPIXELS;i++) {
    
    if(LEDState[i] == 0) {
      if(i != today) {
      pixels.setPixelColor(i, pixels.Color(5,5,5)); // No color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      }
      else {
        pixels.setPixelColor(i, pixels.Color(255, 102, 0)); // Orange.
        pixels.show(); // This sends the updated pixel color to the hardware.
      }
    }
    else if(LEDState[i] == 1) {
      if(i != today) {
      pixels.setPixelColor(i, pixels.Color(0,50,0)); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      }
      else {
        pixels.setPixelColor(i, pixels.Color(20,80,60)); // Moderately bright blue color.
        pixels.show(); // This sends the updated pixel color to the hardware.
      }
    }
    else if(LEDState[i] == 2) {
      if(i != today) {
       pixels.setPixelColor(i, pixels.Color(50,0,0)); // Moderately bright red color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      }
      else {
        pixels.setPixelColor(i, pixels.Color(0,0,50)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
      }
    }
    
  }
  
}

void cycleLEDs() {
    //reset all leds to 0
    for(int i=0;i<NUMPIXELS;i++){
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      LEDState[i] = 0;
      pixels.setPixelColor(i, pixels.Color(0,gcolor,0)); // Moderately bright green color.
  
      pixels.show(); // This sends the updated pixel color to the hardware.
  
      delay(delayval); // Delay for a period of time (in milliseconds).
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // off.
      pixels.show(); // This sends the updated pixel color to the hardware.
  }
}

void loop() {

  DateTime now = RTC.now();
  hours = now.hour();
  minutes = now.minute();
  seconds = now.second();
  weekdays = now.dayOfWeek();
  dayOfMonth = now.day();
  month = now.month();  
  
  //convert to 12-hour time
  if(hours > 12) {
    hours = hours-12; 
  }

  //adjust hour for DLST
  if(daylightsavings) { 
    if(hours > 0) {
      hours = hours-1;
    }
    else {
      hours = 12;
    }
  }
 
  //make it all one number and display
  int displayTime = hours*100 + minutes;
  int displayDate = month*100 + dayOfMonth;
  
  matrix.print(displayDate, DEC); //Just show the date
  matrix.drawColon(false);
  matrix.writeDisplay();  //update display

  // check reset button
   bool resetState = digitalRead(SWITCHPIN);
   if(resetState == LOW) {
          //reset LEDs
          cycleLEDs();
        }
      else {
        //
      }
  
  // Get current weekday button state.
  for(int i =0; i<8;i++) {
    temp_buttonState[i] = digitalRead(buttonPins[i]);
    delay(20); //debounce delay
    buttonState[i] = digitalRead(buttonPins[i]);
    if(temp_buttonState[i] == buttonState[i]) {
      //if button pressed
      if(buttonState[i] == LOW) {
        //if already green (1), then change, else set to green (1)
        if(LEDState[i] == 1) {
          LEDState[i] = 0;
        }
        else {
          LEDState[i] = 1;
        }
      }
    }
  }
  showLEDs(weekdays);
}
