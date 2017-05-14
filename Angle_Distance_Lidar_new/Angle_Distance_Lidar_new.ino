
#include <Wire.h>
#include <ros.h>
#include <std_msgs/String.h>
#define USE_USBCON


#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    check               300           // Defined range of obstacle 


#include <Servo.h>

#include <SoftwareSerial.h> 


Servo myservo;

ros::NodeHandle nh;

std_msgs::String str_msg;
ros::Publisher sensor("sensor", &str_msg);

int pos =0 ;         // Position of the servo (degress, [0, 180])//45
//pos=pos+30;
int distance = 0;    // Distance measured
int count=1;
int m,n;
int values_right[30];
int values_left[30];

char left_message[18]= "left";
char right_message[18] = "right";

void setup()
{
  // Serial output
  Serial.begin(9600);
  Serial.println("< START >");
  
  // Servo control
  myservo.attach(9);
  
  // LIDAR control
  Wire.begin(); // join i2c bus

  nh.initNode();
  nh.advertise(sensor);
}

// Get a measurement from the LIDAR Lite
int lidarGetRange(void)
{
    int val = -1;
    
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
    Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
    Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
    Wire.endTransmission(); // stop transmitting

    nh.spinOnce();
    delay(20); // Wait 20ms for transmit
  
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
    Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
    Wire.endTransmission(); // stop transmitting

    nh.spinOnce();
    delay(20); // Wait 20ms for transmit
    
    Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite
  
    if(2 <= Wire.available()) // if two bytes were received
    {
      val = Wire.read(); // receive high byte (overwrites previous reading)
      val = val << 8; // shift high byte to be high 8 bits
      val |= Wire.read(); // receive low byte as lower 8 bits
      //count++;
      nh.spinOnce();
    }
    nh.spinOnce();
    
    return val;
}

/*void serialPrintRange(int pos,int distance,int x)
{
    //Serial.print("Position (deg): ");
    Serial.print(pos);
    //Serial.print("\t\tDistance (cm): ");
    Serial.print(",");
    Serial.print(distance);
    Serial.print(",");
    Serial.println(x);
}*/

void serialPrintRange(int pos,int distance)
{
    Serial.print("Position (deg): ");
    Serial.print(pos);
    Serial.print("\t\tDistance (cm): ");
    Serial.print(distance);
    
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
        //serialPrintRange(pos, distance,count);
        //serialPrintRange(pos, distance);
          m++;
          nh.spinOnce();
          //Serial.print("m");
          //Serial.print(m);
          delay(100);
          nh.spinOnce();
  //      if(pos==135){
  //        count=count+2;
  //      }
      }
    
    for(pos = 135; pos>= 0; pos-=5)
    {
        myservo.write(pos);
        distance = lidarGetRange();
        values_left[n]=distance;
      //serialPrintRange(pos, distance,count);
      //serialPrintRange(pos, distance);
        n++;
        nh.spinOnce();
        //Serial.print("n");
        //Serial.print(n);
        delay(100);
        nh.spinOnce();
//        if(pos==0){
//        count=count+2;
//      }
    }

//     LEFT AND RIGHT
    for(int i=0;i<30;i++)
    {
       /*Serial.print("\nvalues: "); 
       Serial.print(values_right[i]);*/
       if(values_right[i] <= check){
          str_msg.data = right_message;
          sensor.publish(&str_msg);
          delay(1000);
          Serial.print("\nObstacle detected in right"); 
          Serial.print("\t\tvalue (cm):");
          Serial.print(values_right[i]);
        }
        nh.spinOnce();
        
        /*  Serial.print("\nvaluesl: "); 
        Serial.print(values_left[j]);*/
        if(values_left[i] <= check){
         str_msg.data = left_message;
         sensor.publish( &str_msg );
      
          Serial.print("\nObstacle detected in left"); 
          Serial.print("\t\tvalue (cm):");
          Serial.print(values_left[i]);
        }
        nh.spinOnce();
   }

  //str_msg.data = left_message;
  //sensor.publish( &str_msg );
  nh.spinOnce();
  delay(500);
}

