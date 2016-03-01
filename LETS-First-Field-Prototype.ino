// math to calculate temp
#include <math.h>

// clock config on data logger (DK Data logger shield)
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

// sd config
#include <SPI.h>
#include <SD.h>

// dk data logger chip select
const int chipSelect = 10;
unsigned long LONG_DELAY_MS = 36000000  // 1 hour


// use 2 digits for time and date output;
void print2digits(int number) {
 if (number >= 0 && number < 10) {
    Serial.write('0');
  
  Serial.print(number);
}

// thermistor conversion from analog input to temp in C
double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1)));
  //   = log(10000.0/(1024.0/RawADC-1)) // for pull-up configuration
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;                 // Convert Kelvin to Celcius
  //Temp = (Temp * 9.0)/ 5.0 + 32.0;    // Convert Celcius to Fahrenheit
  return Temp;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);

  
  Serial.println("LAS LETS Pole");
  Serial.println("-------------");

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop() {  

  // make a string for assembling the data to log:
  String dataString = "";

  // date Time section
  tmElements_t tm;

  if (RTC.read(tm)) {
    Serial.print("Date: ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.print(", Time: ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", ");
    
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
    }
  }

  //Temperature section
  // TODO: Add AVERAGES for each TEMP measurement
  Serial.print("Temp0: ");
  Serial.print(Thermistor(analogRead(0)));  // display Centigrade
  Serial.print(", Temp1: ");
  Serial.print(Thermistor(analogRead(1)));  // display Centigrade
  Serial.print(", Temp2: ");
  Serial.print(Thermistor(analogRead(2))); 
  Serial.print(", Temp3: ");
  Serial.println(Thermistor(analogRead(3))); 

  // WRITE DATA TO SD DISK
  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ",";
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  // WAIT 1 hour or whatever john says
  // wait section -- 15 minutes 
  // delay(60000); // works 1 minute
  // delay(300000); // works 5 minutes
  // delay(600000); // 10 minutes
  // delay(1200000);//20minutes
  // delay(1800000);//30minutes

  // VERY LONG DELAYS - 4294967295ms (2^32-1) or 49 days
  // http://arduino.stackexchange.com/questions/3881/very-long-delay-possible
  // unsigned long LONG_DELAY_MS = 9000000  // 15 minutes
  // SAFELY WAIT 1 HOUR
  unsigned long startMillis = millis();
  while (millis() - startMillis < LONG_DELAY_MS);
}
