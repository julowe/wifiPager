#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
 
// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif

#define VBATPIN A7 //same as BUTTON_A...

unsigned long previousMillisScreen = 0;        // will store last time screen was updated
const long intervalScreen = 1000*60*5;           // ms*s*m interval at which to update screen
//const long intervalScreen = 1000;           // ms interval at which to update screen
unsigned long previousMillisRefresh = 0;           // time since refresh in ms
int previousMillisRefreshCounter = 0;    //counter of minutes/iterations after refresh


void setup() {
  //set up buttons on oled shielf
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  //DEBUG
  // initialize digital pin 13 as an output.
  //pinMode(13, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
 
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK); //set text white, background black to overwrite old text
  display.setCursor(0,0);

  //DEBUG
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  //wait 1 second after display boot screen and also waiting for serial to go
  delay(1000);

  //DEBUG - warn self that waiting for serial
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Waiting for serial   connection..."); //spaces for pretty word wrap
    
  //now wait longer if needed to get serial going
  while (!Serial) {
    display.display(); // wait for serial port to connect. Needed for native USB port only
    if(!digitalRead(BUTTON_C)){
      display.setCursor(0,0);
      display.print("Ignoring serial wait. Continuing.");
      display.display();
      delay(1000); //debounce c button
      break;
    }
  }
     
  // Clear the buffer.
  display.clearDisplay();
  display.display();
 
  Serial.println("wifi-pager test");

  updateScreenData();
  updateRefreshTimeDisplay(0);
}

void loop() {
  //update millis to now
  unsigned long currentMillis = millis();


  //display default status message/mode
  //TODO make this a function
  if (currentMillis - previousMillisScreen >= intervalScreen) {
    previousMillisScreen = currentMillis;
    previousMillisRefresh = currentMillis;
    previousMillisRefreshCounter = 0;
    //DEBUG
    Serial.println("Got into loop1");
    Serial.println(currentMillis);

    updateScreenData();
    updateRefreshTimeDisplay(previousMillisRefreshCounter);
  
  }

  //display refreshed time
  if (currentMillis - previousMillisRefresh >= 60000) { //hardcoded 1000ms * 60 seconds for one minute
    previousMillisRefresh = currentMillis;
    ++previousMillisRefreshCounter;

    //tack on refresh counter/timer
    updateRefreshTimeDisplay(previousMillisRefreshCounter);
  }

//  if(!digitalRead(BUTTON_A)) display.print("A");
  if(analogRead(BUTTON_A) < (0.2*1024/3.3/2)){
    display.print("A");
  }
  if(!digitalRead(BUTTON_B)){
    // reset display
    display.clearDisplay();
    display.display();
    display.setCursor(0,0);
    
    //print message
    display.print("Refreshing...");
    display.display();
    delay(750); //wait so it can actualyl be read
    //reset interval in which to refresh screen
    previousMillisScreen = currentMillis;
    previousMillisRefresh = currentMillis;
    previousMillisRefreshCounter = 0;

    //Update screen
    updateScreenData();
    updateRefreshTimeDisplay(previousMillisRefreshCounter);
  }
  if(!digitalRead(BUTTON_C)){
    // Clear the buffer.
    display.clearDisplay();
    display.display();
    display.setCursor(0,0);
    display.print("C");
    display.print(analogRead(BUTTON_A)*2*3.3/1024);
  }
  delay(10);
  yield();
  display.display();

  //DEBUG
//  float measuredvbat = analogRead(VBATPIN);
//  measuredvbat *= 2;    // we divided by 2, so multiply back
//  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
//  measuredvbat /= 1024; // convert to voltage
//  Serial.print("VBat: " ); Serial.println(measuredvbat);

}

void updateScreenData(){
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.print("Battery voltage is: ");
  display.print(analogRead(BUTTON_A)*2*3.3/1024);
}

void updateRefreshTimeDisplay(int x){
  //tack on refresh counter/timer
  //DEBUG
  //display.setCursor(0,16);
  //display.print("TlTLTLTLTL"); //test of lines at top and bottom of text line to see no overlaps
  display.setCursor(0,24);
  display.print("Updated ");
  display.print(x);
  if(x == 1){
    display.print(" min ago "); //don't forget extra space at end to overwrite text correctly
  } else {
    display.print(" mins ago");
  }
  display.display();
}
