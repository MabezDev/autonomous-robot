/*
 * Autonomous Robot (23/06/16)
 * 
 * 
 * 
 * Todo:
 *  - Add take over mode where we can control it over wifi or bluetooth or nrf's
 *      - Will need ack packets from the robot top the commander as there a re blocking loops in this(See : sweep())
 * 
 */


#include <Servo.h>
#include <NewPing.h>

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     4  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 350 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define RIGHT_FORWARD 11
#define RIGHT_REVERSE 6
#define LEFT_FORWARD 3
#define LEFT_REVERSE 5 //cant use 9  and 10 as it conflicts with the servo lib
#define FORWARD_POSITION 90

#define SPEED 0.125 //calculated for this specific robot in m/s
#define RADIUS 0.085 //calculated for this specific robot in m

int pos = 0;
int sonarArray[37];

Servo myservo;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


void setup(){
  Serial.begin(9600);
  pinMode(RIGHT_FORWARD,OUTPUT);
  pinMode(RIGHT_REVERSE,OUTPUT);
  pinMode(LEFT_FORWARD,OUTPUT);
  pinMode(LEFT_REVERSE,OUTPUT);

  myservo.attach(9);
  myservo.write(FORWARD_POSITION);
  delay(300);
}

void loop() {
  int distanceInFront = sonar.ping_cm();
  Serial.println(distanceInFront);
  if(distanceInFront < 20 && distanceInFront!= 1149){ // if an object is closer than 20 cm sweep then choose a new direction and checking if its a valid ping (returns unsigned -1 which is 1149), I changed this in the library
    //stop
    drive(RIGHT_FORWARD,0);
    drive(LEFT_FORWARD,0);
    //check surroundings
    Serial.println("Obstacle detected.");
    Serial.println("Beginning Sweep...");
    sweep();
    Serial.println("Choosing best direction...");
    int degrees = chooseDirection();
    Serial.print("Heading ");
    Serial.print(degrees);
    Serial.println(" degrees from current position.");

    //test pivots working perfectly
    pivot(45);

    delay(5000);

    pivot(180);

    delay(5000);

    pivot(270);
  } else {
    //drive(7,100);
    //drive(LEFT_FORWARD,100);
    //Serial.print("Heading FORWARD at ");
    //Serial.print((100.0/255.0) * 100);
    //Serial.println("% speed.");
  }
  delay(50);
 
  
}

int chooseDirection(){
  //find groups of lots of space (groups of 3 over a hundered cm's in a row) and return the degree where that is ( i * 5 will give a degree from 0 to 180 in 5 increment steps)
  return 0;
}


void pivot(int degrees){
  /*
   * t = d/s, arc length = (2* PI * r * theta), radians = degrees/360
   *  We measure speed to be 0.125 m/s (simple distance of time test)
   *  We measure r to be 0.085m (distance wheel to wheel over 2)
   *  
   *  Combine the equations above to find time:
   *  
   *  t = ((2 * PI * 0.085) * (degrees/360)) / (0.125)
   *  
   *  This gives the time the motors should be on in seconds
   *  
   *  x1000 to convert to milliseconds for out delay function
   * 
   * Doesnt currently account for acceleration
   */
  bool clockwise = true;
  if(degrees > 180){
    degrees -= 180;
    clockwise = false; 
  }
  float time = ((2 * PI * 0.085 * (degrees/360.000))/(0.225) + 0.1); // 0.1 is estimated acceleration time
  if(degrees > 90){
    time += 0.1;
  }
  int timemS = time * 1000;
  
  Serial.print("Time to run motors to reach ");
  Serial.print(degrees);
  Serial.print(" from current position: ");
  Serial.print(timemS);
  Serial.print(" milliseconds");
  if(clockwise){
    Serial.println(" in the clockwise direction.");
  } else {
    Serial.println(" in the anti-clockwise direction.");
  }

  Serial.println("Starting pivot");

  if(clockwise){
    drive(RIGHT_REVERSE,100);
    drive(LEFT_FORWARD,100);
    delay(timemS);
    drive(RIGHT_REVERSE,0);
    drive(LEFT_FORWARD,0);
  } else {
    drive(RIGHT_FORWARD,100);
    drive(LEFT_REVERSE,100);
    delay(timemS);
    drive(RIGHT_FORWARD,0);
    drive(LEFT_REVERSE,0);
  }

  Serial.println("Pivot complete!");
}

void sweep(){
  myservo.write(0);
  delay(300);
  int index = 0;
  for(pos = 0; pos <= 180; pos += 5) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 5 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    sonarArray[index] = sonar.ping_cm();
    index++;
    delay(40);                       // waits 15ms for the servo to reach the position 
  }
  myservo.write(FORWARD_POSITION);
  delay(300);

  Serial.println("Sweep Complete!");
  Serial.println("Sweep Data: ");
  for(int i = 0; i <37; i ++){
    Serial.print("Degree: ");
    Serial.print(i*5);
    Serial.print(" Distance: ");
    Serial.println(sonarArray[i]);
  }
  Serial.println("End of data.");
}



void drive(int pin, int pwm){
  analogWrite(pin,pwm);
}

