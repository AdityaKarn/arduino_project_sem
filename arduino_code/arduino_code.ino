
#include "HX711.h"  //You must have this library in your arduino library folder
#include <SoftwareSerial.h>
#include "RunningAverage.h"

#define DOUT  3
#define CLK  2

SoftwareSerial BT(10, 11);
RunningAverage myRA(15);
int samples = 0;

HX711 scale;

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
// float calibration_factor = -96650; //-106600 worked for my 40Kg max scale setup 
float calibration_factor = -344650;

// setup
void setup() {
  delay(250);
  scale.begin(DOUT, CLK);
  Serial.begin(9600);
  
  BT.begin(9600);
  
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}


//loop
void loop() {
  // scale.set_scale(calibration_factor); //Adjust to this calibration factor
  scale.set_scale(calibration_factor);

  float load_cell_data = scale.get_units();

  // this is to give grams precision, so now it goes from 0 to 10000 (10kg) grams
  load_cell_data = load_cell_data*1000;

  if (load_cell_data < 0) {
    load_cell_data = +0;
  }

  myRA.addValue(load_cell_data);
  samples++;

  float smotthed_load_value = myRA.getAverage();
  
  String data = String("{") + "'weight': '" + String(smotthed_load_value, 4) + "', 'unit': 'g'}";
  BT.println(data);
  Serial.println(data);

  // read bluetooth commands
  if (BT.available()) {
    char command = BT.read();
    
    if(command == 't') {
      Serial.println("tared");      
      scale.tare();  //Reset the scale to zero
    }
  }

  if(Serial.available()) {
    char temp = Serial.read();
    
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == 's')
      calibration_factor += 100;  
    else if(temp == 'x')
      calibration_factor -= 100;  
    else if(temp == 'd')
      calibration_factor += 1000;  
    else if(temp == 'c')
      calibration_factor -= 1000;
    else if(temp == 'f')
      calibration_factor += 10000;  
    else if(temp == 'v')
      calibration_factor -= 10000;  
    else if(temp == 't') {
      Serial.println("tared");
      scale.tare();  //Reset the scale to zero
    }
    if (isDigit(temp)) {  // tests if myChar is a digit
      calibration_factor += temp; 
}
  }
}
//=============================================================================================
