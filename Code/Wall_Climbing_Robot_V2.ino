#include <Servo.h>

char command = 'S';
char prevCommand = 'A';
int velocity = 0;   
boolean Isfan=false;
unsigned long timer0 = 2000;  //Stores the time (in millis since execution started) 
unsigned long timer1 = 0;  //Stores the time when the last command was received from the phone

// Arduino pins for the shift register
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

// 8-bit bus after the 74HC595 shift register 
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_B 0
#define MOTOR4_A 6

// Arduino pins for the PWM signals.
#define MOTOR1_PWM 11
#define MOTOR2_PWM 3
#define MOTOR3_PWM 6
#define MOTOR4_PWM 5
#define SERVO1_PWM 10
#define SERVO2_PWM 9

// Codes for the motor function.
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4


// Declare classes for Servo connectors of the MotorShield.
Servo myservo;
Servo servo_2;
void setup() {
Serial.begin(9600);
  // Use the default "Servo" library of Arduino.
  // Attach the pin number to the servo library.
  // This might also set the servo in the middle position.
  myservo.attach(SERVO1_PWM);
  servo_2.attach(SERVO2_PWM);
  myservo.writeMicroseconds(1000);
}

void loop(){
  if(Serial.available() > 0){ 
    timer1 = millis();   
    prevCommand = command;
    command = Serial.read(); 
    //Change pin mode only if new command is different from previous.   
    if(command!=prevCommand){
      //Serial.println(command);
      switch(command){
      case 'F':  
        Forward_4W();
        break;
      case 'B':  
        Back_4W();
        break;
      case 'L':  
        Left_4W();
        break;
      case 'R':
        Right_4W();  
        break;
      case 'S':  
        Stopped_4W();
        break; 
      case 'I':  //FR  
        //yellowCar.ForwardRight_4W(velocity);
        break; 
      case 'J':  //BR  
        //yellowCar.BackRight_4W(velocity);
        break;        
      case 'G':  //FL  
        //yellowCar.ForwardLeft_4W(velocity);
        break; 
      case 'H':  //BL
        //yellowCar.BackLeft_4W(velocity);
        break;
      case 'W':  //Font ON 
        //digitalWrite(pinfrontLights, HIGH);
        break;
      case 'w':  //Font OFF
        //digitalWrite(pinfrontLights, LOW);
        break;
      case 'U':  //Back ON 
        Isfan=true;
        break;
      case 'u':  //Back OFF 
        Isfan=false;
        break; 
      case 'D':  //Everything OFF 
        //digitalWrite(pinfrontLights, LOW);
        
        Stopped_4W();
        break;         
      default:  //Get velocity
        if(command=='q'){
            if(Isfan==false) {
              velocity = 255;  //Full velocity       
            }
            else{
              myservo.writeMicroseconds(2150);
            }
        }
        else{ 
          //Chars '0' - '9' have an integer equivalence of 48 - 57, accordingly.
          if((command >= 48) && (command <= 57)){ 
            //Subtracting 48 changes the range from 48-57 to 0-9.
            //Multiplying by 25 changes the range from 0-9 to 0-225.

            if(Isfan==false) {
              velocity = (command - 48)*25;       
            }
            else{
              int val,v1;
              val=command - 48;
              v1= map(val, 0, 9,1000,2150); //mapping val to minimum and maximum(Change if needed) 
              myservo.writeMicroseconds(v1);
            }
           
          }
        }
      }
    }
  }
  else{
    timer0 = millis();  //Get the current time (millis since execution started).
    //Check if it has been 500ms since we received last command.
    if((timer0 - timer1)>500){  
      //More tan 500ms have passed since last command received, car is out of range.
      //Therefore stop the car and turn lights off.
      //digitalWrite(pinfrontLights, LOW);
      //digitalWrite(pinbackLights, LOW);
      //yellowCar.Stopped_4W();
    }
  }  
}

void motor(int nMotor, int command, int speed)
{
  int motorA, motorB;

  if (nMotor >= 1 && nMotor <= 4)
  {  
    switch (nMotor)
    {
    case 1:
      motorA   = MOTOR1_A;
      motorB   = MOTOR1_B;
      break;
    case 2:
      motorA   = MOTOR2_A;
      motorB   = MOTOR2_B;
      break;
    case 3:
      motorA   = MOTOR3_A;
      motorB   = MOTOR3_B;
      break;
    case 4:
      motorA   = MOTOR4_A;
      motorB   = MOTOR4_B;
      break;
    default:
      break;
    }

    switch (command)
    {
    case FORWARD:
      motor_output (motorA, HIGH, speed);
      motor_output (motorB, LOW, -1);     // -1: no PWM set
      break;
    case BACKWARD:
      motor_output (motorA, LOW, speed);
      motor_output (motorB, HIGH, -1);    // -1: no PWM set
      break;
    case BRAKE:
      // The AdaFruit library didn't implement a brake.
      // The L293D motor driver ic doesn't have a good
      // brake anyway.
      // It uses transistors inside, and not mosfets.
      // Some use a software break, by using a short
      // reverse voltage.
      // This brake will try to brake, by enabling 
      // the output and by pulling both outputs to ground.
      // But it isn't a good break.
      motor_output (motorA, LOW, 255); // 255: fully on.
      motor_output (motorB, LOW, -1);  // -1: no PWM set
      break;
    case RELEASE:
      motor_output (motorA, LOW, 0);  // 0: output floating.
      motor_output (motorB, LOW, -1); // -1: no PWM set
      break;
    default:
      break;
    }
  }
}

void Forward_4W(){
motor(1, FORWARD, velocity);
motor(2, FORWARD, velocity);
motor(3, FORWARD, velocity);
motor(4, FORWARD, velocity);
}

void Back_4W(){
motor(1, BACKWARD, velocity);
motor(2, BACKWARD, velocity);
motor(3, BACKWARD, velocity);
motor(4, BACKWARD, velocity);
}
void Right_4W(){
motor(1, BACKWARD, velocity);
motor(2, BACKWARD, velocity);
motor(3, FORWARD, velocity);
motor(4, FORWARD, velocity);
}
void Left_4W(){
motor(1, FORWARD, velocity);
motor(2, FORWARD, velocity);
motor(3, BACKWARD, velocity);
motor(4, BACKWARD, velocity);
}
void Stopped_4W(){
motor(1, RELEASE, velocity);
motor(2, RELEASE, velocity);
motor(3, RELEASE, velocity);
motor(4, RELEASE, velocity);
}


// ---------------------------------
// motor_output
//
// The function motor_ouput uses the motor driver to
// drive normal outputs like lights, relays, solenoids, 
// DC motors (but not in reverse).
//
// It is also used as an internal helper function 
// for the motor() function.
//
// The high_low variable should be set 'HIGH' 
// to drive lights, etc.
// It can be set 'LOW', to switch it off, 
// but also a 'speed' of 0 will switch it off.
//
// The 'speed' sets the PWM for 0...255, and is for 
// both pins of the motor output.
//   For example, if motor 3 side 'A' is used to for a
//   dimmed light at 50% (speed is 128), also the 
//   motor 3 side 'B' output will be dimmed for 50%.
// Set to 0 for completelty off (high impedance).
// Set to 255 for fully on.
// Special settings for the PWM speed:
//    Set to -1 for not setting the PWM at all.
//
void motor_output (int output, int high_low, int speed)
{
  int motorPWM;

  switch (output)
  {
  case MOTOR1_A:
  case MOTOR1_B:
    motorPWM = MOTOR1_PWM;
    break;
  case MOTOR2_A:
  case MOTOR2_B:
    motorPWM = MOTOR2_PWM;
    break;
  case MOTOR3_A:
  case MOTOR3_B:
    motorPWM = MOTOR3_PWM;
    break;
  case MOTOR4_A:
  case MOTOR4_B:
    motorPWM = MOTOR4_PWM;
    break;
  default:
    // Use speed as error flag, -3333 = invalid output.
    speed = -3333;
    break;
  }

  if (speed != -3333)
  {
    // Set the direction with the shift register 
    // on the MotorShield, even if the speed = -1.
    // In that case the direction will be set, but
    // not the PWM.
    shiftWrite(output, high_low);

    // set PWM only if it is valid
    if (speed >= 0 && speed <= 255)    
    {
      analogWrite(motorPWM, speed);
    }
  }
}


// ---------------------------------
// shiftWrite
//
// The parameters are just like digitalWrite().
//
// The output is the pin 0...7 (the pin behind 
// the shift register).
// The second parameter is HIGH or LOW.
//
// There is no initialization function.
// Initialization is automatically done at the first
// time it is used.
//
void shiftWrite(int output, int high_low)
{
  static int latch_copy;
  static int shift_register_initialized = false;

  // Do the initialization on the fly, 
  // at the first time it is used.
  if (!shift_register_initialized)
  {
    // Set pins for shift register to output
    pinMode(MOTORLATCH, OUTPUT);
    pinMode(MOTORENABLE, OUTPUT);
    pinMode(MOTORDATA, OUTPUT);
    pinMode(MOTORCLK, OUTPUT);

    // Set pins for shift register to default value (low);
    digitalWrite(MOTORDATA, LOW);
    digitalWrite(MOTORLATCH, LOW);
    digitalWrite(MOTORCLK, LOW);
    // Enable the shift register, set Enable pin Low.
    digitalWrite(MOTORENABLE, LOW);

    // start with all outputs (of the shift register) low
    latch_copy = 0;

    shift_register_initialized = true;
  }

  // The defines HIGH and LOW are 1 and 0.
  // So this is valid.
  bitWrite(latch_copy, output, high_low);

  // Use the default Arduino 'shiftOut()' function to
  // shift the bits with the MOTORCLK as clock pulse.
  // The 74HC595 shiftregister wants the MSB first.
  // After that, generate a latch pulse with MOTORLATCH.
  shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
  delayMicroseconds(5);    // For safety, not really needed.
  digitalWrite(MOTORLATCH, HIGH);
  delayMicroseconds(5);    // For safety, not really needed.
  digitalWrite(MOTORLATCH, LOW);
}

