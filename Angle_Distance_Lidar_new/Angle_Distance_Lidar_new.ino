
#include <Wire.h>

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

#include <Servo.h>

Servo myservo;

int pos =0 ;         // Position of the servo (degress, [0, 180])//45
//pos=pos+30;
int distance = 0;    // Distance measured
int count;
int m,n;
int values_right[30];
int values_left[30];

void setup()
{
  // Serial output
  Serial.begin(9600);
  Serial.println("< START >");
  
  // Servo control
  myservo.attach(9);
  
  // LIDAR control
  Wire.begin(); // join i2c bus
}

// Get a measurement from the LIDAR Lite
int lidarGetRange(void)
{
  int val = -1;
  
  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit
  
  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

  if(2 <= Wire.available()) // if two bytes were received
  {
    val = Wire.read(); // receive high byte (overwrites previous reading)
    val = val << 8; // shift high byte to be high 8 bits
    val |= Wire.read(); // receive low byte as lower 8 bits
    //count++;
  }
  
  return val;
}

void serialPrintRange(int pos, int distance)
{
    Serial.print("Position (deg): ");
    Serial.print(pos);
    Serial.print("\t\tDistance (cm): ");
    Serial.println(distance);
    
    //Serial.print("\tCount: ");
   // Serial.print(count);
}


void loop()
{
  m=0;
  n=0;
  for(pos = 0; pos <=135; pos += 5)
  {
    myservo.write(pos);
    distance = lidarGetRange();
    values_right[m]=distance;
    serialPrintRange(pos, distance);
    m++;
    Serial.print("m");
    Serial.print(m);
    delay(100);
   
  }
  for(pos = 135; pos>= 0; pos-=5)
  {
    myservo.write(pos);
    distance = lidarGetRange();
    values_left[n]=distance;
    serialPrintRange(pos, distance);
    n++;
    Serial.print("n");
    Serial.print(n);
    delay(100);
  }
  for(int i=0;i<30;i++){
     Serial.print("\nvalues: "); 
     Serial.print(values_right[i]);
     delay (10);
    }
    for(int j=0;j<30;j++){
     Serial.print("\nvaluesl: "); 
     Serial.print(values_left[j]);
     delay (10);
    }
  
}

