#include <Servo.h>

/**
 * Author: Aaron Meadows
 * Date: 5/25/18
 * Version: 2.0.0
 * Description: This program will be used on an arduino nano
 * to control a 3D printed robotic arm. The source of the 
 * arm is http://www.eezyrobots.it/eba_mk2.html . 
 * After researching methods of controlling the arm, it was
 * decided that the arm will be controlled using two joysticks
 * and debugged with potentiometers. 
 */
 
//-----MAIN TESTING VARIABLES-----//

const int delayAmount = 20; //Amount to delay the program
const int joyMidRange = 30; // Joystick range of error to eliminate jitter
const int servoJitterRemoval = 5; //Amount that must have changed before the servos change position
boolean debug = false;

//--------------------------------//

//Servo Pins//
const int gripperPIN = 3;
const int leftArmPIN = 9;
const int rightArmPIN = 6;
const int basePIN = 5;
Servo gripper, leftArm, rightArm, base;

//Input Pins//
const int joyLeftXPIN = 0;
const int joyLeftYPIN = 1;
const int joyRightXPIN = 2;
const int joyRightYPIN = 3;

//Joystick Signals//
int joyLeftXSIG;
int joyLeftYSIG;
int joyRightXSIG;
int joyRightYSIG;

//Joystick Values//
const int joyMaxVal = 1023;
const int joyMidVal = joyMaxVal / 2;

//Servo Signals//
int gripperSIG;
int leftArmSIG;
int rightArmSIG;
int baseSIG;

//Old Values//
int oldL;
int oldR;
int oldG;
int oldB;

void setup() {
  //Setup Servos//
  gripper.attach(gripperPIN);
  leftArm.attach(leftArmPIN);
  rightArm.attach(rightArmPIN);
  base.attach(basePIN);

  //Start the serial monitor//
  Serial.begin(9600);
  
  //Set servos to their middle position//
  setMiddle();

  //Move all to their middle position//
  moveAll();
}

void loop() {
  //If debug is true then output signal values
  if (debug)
  {
    outputValues();
  }
  
  //Set the signal values to go to the servos//
  setSignals();

  //Write the signal values to every servo//
  moveAll();

  //Delay the program//
  delay(delayAmount);
}

/**
 * increment()
 * This method will use input to determine if 
 * the signal value can be incresed or decreased, 
 * if possible it will return the incremented value.
 * @param a Amount that the function will increment or decrement 
 * @param joy Joystick signal value to determine increase amount
 * @param minimum Minimun value to return
 * @param maximun Maximum value to return
 */
int increment(int a, int joy, int minimum, int maximum)
{
  //Variable to set increment amount
  int increment; 
  
  //Check if joystick value is left but still above the minimum
  if (joy < joyMidVal - joyMidRange && a >= minimum)
  {
    increment = map(joy, 0, joyMidVal - joyMidRange, -3, -1);
  }
  //Check if joystick value is right but still below the maximum
  else if (joy > joyMidVal + joyMidRange && a <= maximum)
  {
    increment = map(joy, joyMidVal + joyMidRange, joyMaxVal, 1, 3);
  }
  //Otherwise, set the amount to increment to 0
  else
  {
    increment = 0;
  }
  
  //Add value to signal
  a += increment;

  //Return the new signal 
  return a;
}

//Execute all the move methods for each servo
void moveAll()
{
  gripperMove();
  baseMove();
  leftArmMove();
  rightArmMove();
}

//Read joysticks and set signal values and increment if necessary
void setSignals()
{
  //Set the signal values of the joystick//
  readJoysticks();

  //Increment or decrement the signal values if they have avaliable space//
  leftArmSIG = increment(leftArmSIG, joyRightXSIG, 85, 195); //Increment left Arm
  rightArmSIG = increment(rightArmSIG, joyRightYSIG, 35, 115); //Increment right Arm
  gripperSIG = increment(gripperSIG, joyLeftXSIG, 45, 155); //Increment gripper
  baseSIG = increment(baseSIG, joyLeftYSIG, 5, 165); //Increment base
}

//Read joystick analog pins
void readJoysticks()
{
  joyLeftXSIG = analogRead(joyLeftXPIN);
  joyLeftYSIG = analogRead(joyLeftYPIN);
  joyRightXSIG = analogRead(joyRightXPIN);
  joyRightYSIG = analogRead(joyRightYPIN);
}

void leftArmMove()
{  
  //Write only if the value has changed
  if (leftArmSIG < (oldL + servoJitterRemoval) || leftArmSIG > (oldL - servoJitterRemoval))
  {
    leftArm.write(leftArmSIG);
    Serial.println(leftArmSIG);
  }
  
  //Reset old value
  oldL = leftArmSIG;
}

void rightArmMove()
{
  //Write only if the value has changed
  if (rightArmSIG < (oldR + servoJitterRemoval) || rightArmSIG > (oldR - servoJitterRemoval))
  {
    rightArm.write(rightArmSIG);
    Serial.println(rightArmSIG);
  }

  //Reset old value
  oldR = rightArmSIG;
}

void gripperMove()
{
  //Write only if the value has changed
  if (gripperSIG < (oldG + servoJitterRemoval) || gripperSIG > (oldG - servoJitterRemoval))
  {
    gripper.write(gripperSIG);
    Serial.println(gripperSIG);
  }

  //Reset old value
  oldG = gripperSIG;
}

void baseMove()
{
  //Write only if the value has changed more than (oldB + servoJitterRemoval)
  if (baseSIG < (oldB + servoJitterRemoval) || baseSIG > (oldB - servoJitterRemoval)) 
  {
    base.write(baseSIG);
    Serial.println(baseSIG);
  }

  //Reset old value
  oldB = baseSIG;
}

void setMiddle()
{
  gripperSIG = 100;
  baseSIG = 85;
  leftArmSIG = 140;
  rightArmSIG = 65;
}

//Output signal values to the serial monitor//
void outputValues()
{
  setSignals();

  Serial.print("Right Arm:              ");
  Serial.println(rightArmSIG); 
  
  Serial.print("Left Arm:           ");
  Serial.println(leftArmSIG);
  
  Serial.print("Gripper:        ");
  Serial.println(gripperSIG);
  
  Serial.print("Base:       ");
  Serial.println(baseSIG);

}

