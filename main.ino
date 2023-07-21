/*
	Basic Functions

	NOTES:
	- delay(1000) makes the robot wait for 1000 milliseconds.
	- maybe we end up using async functions if they exist in C++?
  - We may need to worry about the orange tape in the middle of the right when using HIGH.
  
  TODO:
  - Sensor input.
	- Strategy implementation.
*/

/*
	Basic input functions.
 
 	TODO:
  	- READ THIS FOR INPUT AND SEARCH SIMILAR STUFF UP FOR BASIC OUTPUT AND INPUT CODE: https://peppe8o.com/ir-sensor-with-arduino-wiring-and-code-explained/#:~:text=In%20the%20beginning%2C%20we%20declare%20the%20ir_sensor_pin%20with,%28ir_sensor_pin%2C%20INPUT%29%3B%20%2F%2F%20Pin%20set%20as%20input%20%7D
  	- what does analogRead(infraredSensorPin) return?
  	- Need to create more instances of each ultrasonic sensor and ir sensor cos we have multiple.
   	- functions for the grouping input.
  	- Detection for where enemy is in between left two and right two sensors.
   	- Detection for otherwise (e.g. enemy is seem on rightmost facing sensor but then disappears - we should still turn right)
	- Detection for sensor groups (e.g. the two ir sensors (also applies to ultrasonic sensors) at the back of the robot both get white input. We should therefore do avoidance maneuvore or just push forward)
*/

#define RING_DIAMETER_CM 120  // Used for maximum distance ultrasonic sensor should detect

// #define FRONT 0  // Used for tracking the last known enemy placement.
// #define LEFT -1
// #define RIGHT 1

#include <Ultrasonic.h>

Ultrasonic ultrasonic_left(1, 0);
Ultrasonic ultrasonic_centre(3, 2);
Ultrasonic ultrasonic_right(5, 4);  // Format is trig pin then echo pin

// I'm unsure which one works so heres both declarations for ir sensors - should just be the first one I hope.
const int FRONT_LEFT_IR = 0;
const int BACK_LEFT_IR = 1;
const int BACK_RIGHT_IR = 2;
const int FRONT_RIGHT_IR = 3;
// #define FRONT_LEFT_IR A0;
// #define BACK_LEFT_IR A1;
// #define BACK_RIGHT_IR A2;
// #define FRONT_RIGHT_IR A3;

// Speeds
const int MAX_SPEED = 255;
const int MIN_SPEED = 0;

// Directions
const int ANTICLOCKWISE = -1;
const int FORWARD = 0;
const int CLOCKWISE = 1;

// Motor Constants (we will have to change these values for our robot)
// Motor A (Left Motor)
const int enA = 8;
const int inA1 = 9;
const int inA2 = 10;

// Motor B (Right Motor)
const int enB = 13;
const int inB1 = 11;
const int inB2 = 12;

struct Motor {
	int en;
	int in1;
	int in2;
	int max_speed;
	int min_speed;
};

const Motor motorA = {enA, inA1, inA2, MAX_SPEED, MIN_SPEED};
const Motor motorB = {enB, inB1, inB2, MAX_SPEED, MIN_SPEED};

// Set all the motor control pins to outputs using pinMode()
void pinSetup() {
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(inA1, OUTPUT);
	pinMode(inA2, OUTPUT);
	pinMode(inB1, OUTPUT);
	pinMode(inB2, OUTPUT);

  pinMode(FRONT_LEFT_IR, INPUT);
  pinMode(BACK_LEFT_IR, INPUT);
  pinMode(BACK_RIGHT_IR, INPUT);
  pinMode(FRONT_RIGHT_IR, INPUT);
}

void setup() {
	pinSetup();
}

// Speed between -MAX_SPEED (negative MAX_SPEED) and +MAX_SPEED (positive MAX_SPEED)
// Negative speed makes the motor spin backwards
void driveSingleWheel(const Motor& selected_motor, int speed) {
	if (speed < 0) {
		digitalWrite(selected_motor.in1, LOW);
		digitalWrite(selected_motor.in2, HIGH);
		analogWrite(selected_motor.en, abs(speed));
	} else {
		digitalWrite(selected_motor.in1, HIGH);
		digitalWrite(selected_motor.in2, LOW);
		analogWrite(selected_motor.en, speed);
	}
}

// Negative speed = backwards
void driveStraight(const Motor& motorA, const Motor& motorB, int speed) {
	driveSingleWheel(motorA, speed);
	driveSingleWheel(motorB, speed);
}

void turnOffMotors() {
	digitalWrite(inA1, LOW);
	digitalWrite(inA2, LOW);
	digitalWrite(inB1, LOW);
	digitalWrite(inB2, LOW);
}

// Turns the robot while it moves forward.
// -1 wheel_pointing_direction will make the robot rotate anticlockwise if it is moving forwards. 
// 0 will not make the wheels move. 1 will make the robot rotate clockwise.
// turn_speed should be an integer value between 0 and MAX_SPEED. 0 turn speed will not make the robot turn. 
// MAX_SPEED turn speed will make the robot turn very quickly on the spot.
// turn_speed in between will make it turn at a slower pace by making the robot still move forward as it turns. 
void forward_turn(const Motor& motorA, const Motor& motorB, int turn_direction, int turn_speed) {
  // if (turn_speed == 0) {  // Probably not needed, but include anyways to make things run faster.
  // 	return;
  // }

	driveSingleWheel(motorA, turn_direction * motorA.max_speed);
	driveSingleWheel(motorB, motorB.max_speed - 2 * turn_direction * turn_speed);
}

void reverse_turn(const Motor& motorA, const Motor& motorB, int turn_direction, int turn_speed) {
	// if (turn_speed == 0) {  // Probably not needed
	// 	return;
	// }

	driveSingleWheel(motorA, -turn_direction * motorA.max_speed);
	driveSingleWheel(motorB, -motorB.max_speed + 2 * turn_direction * turn_speed);
}

// THIS IS ONLY FOR TESTING
void testing_movement() {
	// Drive forward at full speed for 1 second
	driveStraight(motorA, motorB, MAX_SPEED);
	delay(1000);

  // Reverse
	driveStraight(motorA, motorB, -MAX_SPEED);
	delay(500);

	// Stop the robot for 500 milliseconds
	driveStraight(motorA, motorB, 0);
	delay(500);

	// Turn left at 50% speed for 2 seconds
	forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / 2);
	delay(2000);

	// Stop the robot
	driveStraight(motorA, motorB, 0);
	delay(500);

	// Turn right at 75% speed for 3 seconds
	forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 3 / 4);
	delay(3000);

	// Stop the robot
	driveStraight(motorA, motorB, 0);
	delay(500);

  // Turn left whilst moving backwards at 25% speed for 3 seconds
	reverse_turn(motorA, motorB, CLOCKWISE, MAX_SPEED / 4);
	delay(3000);

  // Turn right whilst moving backwards at 10% speed for 3 seconds
	reverse_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / 10);
	delay(3000);

  // TESTING TURNING AT 100% SPEED (I.E. ON THE SPOT)
  // Turn on the spot clockwise at 100% speed for 3 seconds
  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
  delay(3000);

  // Stop the robot
  driveStraight(motorA, motorB, -MAX_SPEED);
  delay(500);

  // Turn on the spot anticlockwise at 100% speed for 3 seconds
  forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
  delay(3000);

  // Stop the robot
  driveStraight(motorA, motorB, 0);
  delay(500);

  // Turn on the spot anticlockwise? at 100% speed for 3 seconds
  reverse_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
  delay(3000);

  // Stop the robot
  driveStraight(motorA, motorB, 0);
  delay(500);

  // Turn on the spot clockwise? at 100% speed for 3 seconds
  reverse_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
  delay(3000);


  // AND JUST TO OBSERVE BEHAVIOUR WHEN TURN SPEED IS 0:
  // Turn the robot clockwise with 0 turn speed (should not turn)
  forward_turn(motorA, motorB, CLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);

  // Turn the robot anticlockwise with 0 turn speed (should not turn)
  forward_turn(motorA, motorB, ANTICLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);

  reverse_turn(motorA, motorB, CLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);

  reverse_turn(motorA, motorB, ANTICLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);
}

void move_to_side_start_sequence() {
  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
  delay(300);  // we might need async functions to detect where the enemy is while this is happening - same goes with other instances where we use delay, or just use a function that returns the time elapsed. such functions should easily exist.
  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 2 / 3);
  delay(1000);
  forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED * 3 / 4);
  delay(500);
}

void main_competition_strategy() {
  delay(5000);  // Rules state we must wait for 5 seconds before moving

  // Past events - used as secondary resource.
  bool enemy_was_to_left = false;  // you might have to also constantly update these in the loop.
  bool enemy_was_to_centre = true;
  bool enemy_was_to_right = false;

  // current statuses - used as primary resource.
  bool enemy_is_to_left = false;
  bool enemy_is_to_centre = true;
  bool enemy_is_to_right = false;

  move_to_side_start_sequence();  // move to side to mess up opponent.

  int distance_ultrasonic_left_cm = ultrasonic_left.read();  // It is better to call this every time to ensure we have most up to date data.
  int distance_ultrasonic_centre_cm = ultrasonic_centre.read();  // we should check for 0 < ring diameter here as some things return negative if nothing detected
  int distance_ultrasonic_right_cm = ultrasonic_right.read();  // YOU MIGHT have to constantly update these in the loop

  int front_left_ir_value = digitalRead(FRONT_LEFT_IR);  // YOU MIGHT have to constantly update these in the loop
  int back_left_ir_value = digitalRead(BACK_LEFT_IR);
  int back_right_ir_value = digitalRead(BACK_RIGHT_IR);
  int front_right_ir_value = digitalRead(FRONT_RIGHT_IR);

  // I CAN PROBABLY GET RID OF THE CHECKS THAT CHECK IF ITS GREATER THAN 0 BECAUSE APPARENTLY NOW THEY REUTRN A VERY BIG VALUE 200000 IF NOTHING IS DETECTED.
  while (true) {
    // Update last enemy detection direction. NOTE: we don't set these to false because we want to track the last known enemy location. We will set these to false later on within certain functions.
    // 'was' are saved statuses of the past used as a secondary resource if the enemy is not currently already in front of our sensors. 'is' are current statuses and are used as the primary resource.
    if (distance_ultrasonic_left_cm < RING_DIAMETER_CM && distance_ultrasonic_centre_cm > 0) {
      enemy_was_to_left = true;
      enemy_is_to_left = true;
    } else {
      enemy_is_to_left = false;
    }

    if (distance_ultrasonic_centre_cm < RING_DIAMETER_CM && distance_ultrasonic_centre_cm > 0) {  // might run into some issues when enemy is right up against our sensor but if our sensor doesnt detect anything then 0 is apparently returned according to chatgpt.
      enemy_was_to_centre = true;
      enemy_is_to_centre = true;
    } else {
      enemy_is_to_centre = false;
    }

    if (distance_ultrasonic_right_cm < RING_DIAMETER_CM && distance_ultrasonic_centre_cm > 0) {
      enemy_was_to_right = true;
      enemy_is_to_right = true;
    } else {
      enemy_is_to_right = false;
    }

    if (enemy_is_to_centre || enemy_is_to_left || enemy_is_to_right) {
      // Check current statuses - priority is given to these and we go off these first.
      // Determine the direction with the least distance
      if (distance_ultrasonic_left_cm < distance_ultrasonic_centre_cm &&
          distance_ultrasonic_left_cm < distance_ultrasonic_right_cm) {
        // Turn left
        forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED * 2 / 3);
      } else if (distance_ultrasonic_right_cm < distance_ultrasonic_centre_cm &&
          distance_ultrasonic_right_cm < distance_ultrasonic_left_cm) {
        // Turn right
        forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 2 / 3);
      } else {
        // Go forward
        driveStraight(motorA, motorB, MAX_SPEED);
      }



      // Check previous statuses. We go off these as a second resource of information if the enemy is not directly in front of our sensors at this moment in time.
    }



    // update distance measurements ultrasonic.
    distance_ultrasonic_left_cm = ultrasonic_left.read();
    distance_ultrasonic_centre_cm = ultrasonic_centre.read();
    distance_ultrasonic_right_cm = ultrasonic_right.read();

    front_left_ir_value = digitalRead(FRONT_LEFT_IR);
    back_left_ir_value = digitalRead(BACK_LEFT_IR);
    back_right_ir_value = digitalRead(BACK_RIGHT_IR);
    front_right_ir_value = digitalRead(FRONT_RIGHT_IR);
  }
}

// Maybe i need to pass in the so called constants defined before into these functions? Or maybe not since they are global scope.
void loop() {
  main_competition_strategy();
	testing_movement();
}
