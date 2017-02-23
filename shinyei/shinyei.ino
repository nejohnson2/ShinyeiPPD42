#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// SD Card Setup --------------------------------------
const int chipSelect = 10;
File dataFile;

// BME280 Setup --------------------------------------
Adafruit_BME280 bme;

// ShinyeiPPD42 Setup --------------------------------------
const int SHINYEI_INPUT_P1 = 2;   // Digital Pin 2 (INT0) - P1 - Particles >0.1
const int SHINYEI_INPUT_P2 = 3;   // Digital Pin 3 (INT1) - P2 - Particles >2.5

volatile unsigned long p1_timer_start = 0;
volatile unsigned long p2_timer_start = 0;
volatile float p1_ratio;
volatile float p2_ratio;
volatile float countP1;
volatile float countP2;
volatile float PM25count;
volatile float PM10count;

unsigned long starttime;
unsigned long sampletime_ms = 5000;

// variables for dust calulations --------------------------------------
const double pi = 3.14159;
const double density = 1.65 * pow(10, 12);
const double r10 = 2.6 * pow(10, -6);
const double vol10 = (4.0 / 3.0) * pi * pow(r10, 3);
const double mass10 = density * vol10;
const double r25 = 0.44 * pow(10, -6);
const double vol25 = (4.0 / 3.0) * pi * pow(r25, 3);
const double mass25 = density * vol25;
const double K = 3531.5;

struct Shinyei {
  volatile unsigned long p1_pulse_time;
  volatile unsigned long p2_pulse_time;
  volatile float concSmall;
  volatile float concLarge;
};

Shinyei data;

void setup() {
  pinMode(SHINYEI_INPUT_P1, INPUT);
  pinMode(SHINYEI_INPUT_P2, INPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.print("Initializing SD card...");
  if (initSD()) {
  }
  Serial.println("card initialized.");

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  starttime = millis();

  // Interrupts for PPD42 --------------------------------------
  attachInterrupt(digitalPinToInterrupt(SHINYEI_INPUT_P1), P1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SHINYEI_INPUT_P2), P2, CHANGE);
}

void loop() {
  if ((millis() - starttime) > sampletime_ms) {
    write_file();
  }
}

////////////////////////////////////////////////////////////////////////////////
// Utility Functions
////////////////////////////////////////////////////////////////////////////////
boolean initSD() {
  // this is the default slave select.  must be set to output
  pinMode(SS, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }
  // Open up the file we're going to log to!
  dataFile = SD.open("datafile.txt", FILE_WRITE);
  if (!dataFile) {
    // Wait forever since we cant write data
    while (1) ;
  }
  return true;
}

void write_file() {
  // prevent interrupts from happening during writing
  detachInterrupt(digitalPinToInterrupt(SHINYEI_INPUT_P1));
  detachInterrupt(digitalPinToInterrupt(SHINYEI_INPUT_P2));

  calcDust(); // calculate ratio/concentration

  String dataString = "";

  dataString += bme.readTemperature();  // celsius
  dataString += ",";
  dataString += bme.readHumidity();     // relative humidity
  dataString += ",";
  dataString += bme.readPressure();     // pascals
  dataString += ",";
  dataString += data.p1_pulse_time;     // LPO
  dataString += ",";
  dataString += data.p2_pulse_time;     // LPO
  dataString += ",";
  dataString += data.concLarge;         // pm10 ug/m3
  dataString += ",";
  dataString += data.concSmall;         // pm2.5 ug/m3

  Serial.println(dataString);
  dataFile.println(dataString);   // write data to sd card
  dataFile.flush();               // clear buffer

  // reset values
  data.p1_pulse_time = 0;
  data.p2_pulse_time = 0;

  starttime = millis();

  attachInterrupt(digitalPinToInterrupt(SHINYEI_INPUT_P1), P1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SHINYEI_INPUT_P2), P2, CHANGE);
}
////////////////////////////////////////////////////////////////////////////////
// ShinyeiPPD42
////////////////////////////////////////////////////////////////////////////////
/*
    Generate PM10 and PM2.5 counts
*/
void calcDust() {
  // Generates PM10 and PM2.5 count from LPO.
  // Derived from code created by Chris Nafis
  // http://www.howmuchsnow.com/arduino/airquality/grovedust/

  p1_ratio = data.p1_pulse_time / (sampletime_ms * 10.0);
  p2_ratio = data.p2_pulse_time / (sampletime_ms * 10.0);

  // from shinyei datasheet
  countP1 = 1.1 * pow(p1_ratio, 3) - 3.8 * pow(p1_ratio, 2) + 520 * p1_ratio + 0.62;
  countP2 = 1.1 * pow(p2_ratio, 3) - 3.8 * pow(p2_ratio, 2) + 520 * p2_ratio + 0.62;

  PM10count = countP2;
  PM25count = countP1 - countP2;

  // Calculate ug/m3 from counts.  Used in Dustduino
  // and originally developed by researchers at Drexel
  // http://github.com/nejohnson2/ShinyeiPPD42

  // begins PM10 mass concentration algorithm
  data.concLarge = (PM10count) * K * mass10;

  // next, PM2.5 mass concentration algorithm
  data.concSmall = (PM25count) * K * mass25;
}

/*
   Interrupt for P1 >0.1
*/
void P1() {
  if (digitalRead(SHINYEI_INPUT_P1) == LOW)  {
    p1_timer_start = micros();
  }
  else  {
    //only worry about this if the timer has actually started
    if (p1_timer_start != 0)
    {
      data.p1_pulse_time += micros() - p1_timer_start;
      p1_timer_start = 0; //restart the timer
    }
  }
}

/*
   Interrupt for P2 >2.5
*/
void P2() {
  if (digitalRead(SHINYEI_INPUT_P2) == LOW)  {
    p2_timer_start = micros();
  }
  else  {
    //only worry about this if the timer has actually started
    if (p2_timer_start != 0)
    {
      data.p2_pulse_time += micros() - p2_timer_start;
      p2_timer_start = 0; //restart the timer
    }
  }
}
