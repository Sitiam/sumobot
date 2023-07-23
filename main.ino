/*
	Basic Functions

	NOTES:
  - We may need to worry about the orange tape in the middle of the right when using HIGH.
  
  TODO:
  - Sensor input.
	- Strategy implementation.
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

// struct Ultrasonic {
//   int trig_pin;
//   int echo_pin;
// };

// const Ultrasonic ultrasonic_left = {7, 6};
// const Ultrasonic ultrasonic_centre = {3, 2};
// const Ultrasonic ultrasonic_right = {5, 4};  // Format is trig pin then echo pin

Ultrasonic ultrasonic_left(7, 6);
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
const int inB2 = 11;
const int inB1 = 12;

struct Motor {
	int en;
	int in1;
	int in2;
	int max_speed;
	int min_speed;
};

const Motor motorA = {enA, inA1, inA2, MAX_SPEED, MIN_SPEED};
const Motor motorB = {enB, inB1, inB2, MAX_SPEED, MIN_SPEED};

void setup() {
  Serial.begin(9600);
	
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

  // pinMode(ultrasonic_left.trig_pin, OUTPUT); // Sets the trigPin as an Output
  // pinMode(ultrasonic_left.echo_pin, INPUT); // Sets the echoPin as an Input
  // pinMode(ultrasonic_centre.trig_pin, OUTPUT); // Sets the trigPin as an Output
  // pinMode(ultrasonic_centre.echo_pin, INPUT); // Sets the echoPin as an Input
  // pinMode(ultrasonic_right.trig_pin, OUTPUT); // Sets the trigPin as an Output
  // pinMode(ultrasonic_right.echo_pin, INPUT); // Sets the echoPin as an Input
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
  if (turn_direction == CLOCKWISE) {
    driveSingleWheel(motorA, motorA.max_speed);
    driveSingleWheel(motorB, motorB.max_speed - 2 * turn_speed);
  } else {
    driveSingleWheel(motorA, motorA.max_speed - 2 * turn_speed);
    driveSingleWheel(motorB, motorA.max_speed);
  }
}

void reverse_turn(const Motor& motorA, const Motor& motorB, int turn_direction, int turn_speed) {
  if (turn_direction == CLOCKWISE) {
    driveSingleWheel(motorA, -motorA.max_speed);
    driveSingleWheel(motorB, -motorB.max_speed + 2 * turn_speed);
  } else {
    driveSingleWheel(motorA, -motorA.max_speed + 2 * turn_speed);
    driveSingleWheel(motorB, -motorA.max_speed);
  }
}

// int detect_distance(const Ultrasonic& sensor) {
//   // Clears the trigPin
//   digitalWrite(sensor.trig_pin, LOW);
//   delayMicroseconds(2);
//   // Sets the trigPin on HIGH state for 10 micro seconds
//   digitalWrite(sensor.trig_pin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(sensor.trig_pin, LOW);
//   // Reads the echoPin, returns the sound wave travel time in microseconds
//   long duration = pulseIn(sensor.echo_pin, HIGH);
//   // Calculating the distance
//   return duration * 0.034 / 2;
// }

// THIS IS ONLY FOR TESTING
void testing_movement() {
	// // Drive forward at full speed for 1 second
	// driveStraight(motorA, motorB, MAX_SPEED);
	// delay(5000);

  // driveStraight(motorA, motorB, 0);
  // delay(5000);

  Serial.println("hi");

  driveSingleWheel(motorA, motorA.max_speed);
  delay(500);
  driveSingleWheel(motorA, motorA.max_speed * 0.75);
  delay(500);
  driveSingleWheel(motorA, motorA.max_speed * 0.5);
  delay(500);
  driveSingleWheel(motorA, motorA.max_speed * 0.25);
  delay(500);
  driveSingleWheel(motorA, motorA.max_speed * 0);
  delay(50000);


  // // Reverse
	// driveStraight(motorA, motorB, -MAX_SPEED);
	// delay(1000);

  // driveStraight(motorA, motorB, 0);
  // delay(5000);


  // CHECK FOR BOTH CLOCKWISE AND ANTICLOCKWISE IN THESE FUNCTIONS.  // MUST DO THIS FOR ALL, NOT JUST THESE FOUR. I RAN OUT OF TIME TONIGHT.
	forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.9);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.8);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);

  
  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.7);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.6);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.5);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.4);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.3);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.2);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 0.1);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(1000);


  forward_turn(motorA, motorB, CLOCKWISE, 0);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(15000);



	// Turn left at 100% speed for 2 seconds
	forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(5000);

  // Turn left at 50% speed for 2 seconds
	forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / 2);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(5000);

  // Turn left at 0% speed for 2 seconds
	forward_turn(motorA, motorB, ANTICLOCKWISE, 0);
	delay(2000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


	// Turn right at 75% speed for 3 seconds
	forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 3 / 4);
	delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);

  
	// Turn right at 25% speed for 3 seconds
	forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED * 1 / 4);
	delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Turn left whilst moving backwards at 25% speed for 3 seconds
	reverse_turn(motorA, motorB, CLOCKWISE, MAX_SPEED / 4);
	delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Turn right whilst moving backwards at 10% speed for 3 seconds
	reverse_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / 10);
	delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // TESTING TURNING AT 100% SPEED (I.E. ON THE SPOT)
  // Turn on the spot clockwise at 100% speed for 3 seconds
  forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Stop the robot
  driveStraight(motorA, motorB, -MAX_SPEED);
  delay(500);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Turn on the spot anticlockwise at 100% speed for 3 seconds
  forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Turn on the spot anticlockwise? at 100% speed for 3 seconds
  reverse_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Turn on the spot clockwise? at 100% speed for 3 seconds
  reverse_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // AND JUST TO OBSERVE BEHAVIOUR WHEN TURN SPEED IS 0:
  // Turn the robot clockwise with 0 turn speed (should not turn)
  forward_turn(motorA, motorB, CLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  // Turn the robot anticlockwise with 0 turn speed (should not turn)
  forward_turn(motorA, motorB, ANTICLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  reverse_turn(motorA, motorB, CLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  reverse_turn(motorA, motorB, ANTICLOCKWISE, 0);
  delay(3000);

  driveStraight(motorA, motorB, 0);
  delay(5000);


  driveStraight(motorA, motorB, MAX_SPEED / 10);
  delay(500);

  driveStraight(motorA, motorB, 0);
  delay(5000);
}

void update_distance_and_direction_readings(bool *enemy_was_to_left, bool *enemy_was_to_centre, bool *enemy_was_to_right, bool *enemy_is_to_left, bool *enemy_is_to_centre, bool *enemy_is_to_right,
                                            int *distance_ultrasonic_left_cm, int *distance_ultrasonic_centre_cm, int *distance_ultrasonic_right_cm, int *old_distance_ultrasonic_left_cm, int *old_distance_ultrasonic_centre_cm, int *old_distance_ultrasonic_right_cm,
                                            bool *front_left_ir_value, bool *back_left_ir_value, bool *back_right_ir_value, bool *front_right_ir_value) {
  *distance_ultrasonic_left_cm = ultrasonic_left.read();  // It is better to call this every time to ensure we have most up to date data.
  *distance_ultrasonic_centre_cm = ultrasonic_centre.read();  // we should check for 0 < ring diameter here as some things return negative if nothing detected
  *distance_ultrasonic_right_cm = ultrasonic_right.read();  // YOU MIGHT have to constantly update these in the loop
  // Update last enemy detection direction. NOTE: we don't set these to false because we want to track the last known enemy location. We will set these to false later on within certain functions.
  // 'was' are saved statuses of the past used as a secondary resource if the enemy is not currently already in front of our sensors. 'is' are current statuses and are used as the primary resource.
  
  if (*distance_ultrasonic_left_cm < RING_DIAMETER_CM || *distance_ultrasonic_centre_cm < RING_DIAMETER_CM || *distance_ultrasonic_right_cm < RING_DIAMETER_CM) {
    if (*distance_ultrasonic_left_cm < *distance_ultrasonic_centre_cm && *distance_ultrasonic_left_cm < *distance_ultrasonic_right_cm) {
      *old_distance_ultrasonic_left_cm = *distance_ultrasonic_left_cm;
      *enemy_was_to_left = true;
      *enemy_is_to_left = true;
      *enemy_was_to_right = false;
      *enemy_is_to_right = false;
      *enemy_was_to_centre = false;
      *enemy_is_to_centre = false;

      *old_distance_ultrasonic_right_cm = RING_DIAMETER_CM + 20;
      *old_distance_ultrasonic_centre_cm = RING_DIAMETER_CM + 20;
      
    } else if (*distance_ultrasonic_right_cm < *distance_ultrasonic_centre_cm && *distance_ultrasonic_right_cm < *distance_ultrasonic_left_cm) {
      *old_distance_ultrasonic_right_cm = *distance_ultrasonic_right_cm;
      *enemy_was_to_right = true;
      *enemy_is_to_right = true;
      *enemy_was_to_left = false;
      *enemy_is_to_left = false;
      *enemy_was_to_centre = false;
      *enemy_is_to_centre = false;

      *old_distance_ultrasonic_left_cm = RING_DIAMETER_CM + 20;
      *old_distance_ultrasonic_centre_cm = RING_DIAMETER_CM + 20;
    } else if (*distance_ultrasonic_centre_cm < *distance_ultrasonic_left_cm && *distance_ultrasonic_centre_cm < *distance_ultrasonic_right_cm) {
      *old_distance_ultrasonic_centre_cm = *distance_ultrasonic_centre_cm;
      *enemy_was_to_centre = true;
      *enemy_is_to_centre = true;
      *enemy_was_to_left = false;
      *enemy_is_to_left = false;
      *enemy_was_to_right = false;
      *enemy_is_to_right = false;
      
      *old_distance_ultrasonic_left_cm = RING_DIAMETER_CM + 20;
      *old_distance_ultrasonic_right_cm = RING_DIAMETER_CM + 20;
    }
  }
  *front_left_ir_value = digitalRead(FRONT_LEFT_IR);  // YOU MIGHT have to constantly update these in the loop
  *back_left_ir_value = digitalRead(BACK_LEFT_IR);
  *back_right_ir_value = digitalRead(BACK_RIGHT_IR);
  *front_right_ir_value = digitalRead(FRONT_RIGHT_IR);
}

void main_competition_strategy() {
  // delay(5000);  // Rules state we must wait for 5 seconds before moving -- FLIPPING THE SWITCH AUTOMATICALLY PAUSES IT FOR a few SECONDS - MAYBE 3500 INSTEAD??? - BUT WE WILL HAVE TO TIME AND TEST OURSELVES

  // Past events - used as secondary resource.
  bool enemy_was_to_left = false;  // you might have to also constantly update these in the loop.
  bool enemy_was_to_centre = false;
  bool enemy_was_to_right = false;

  // current statuses - used as primary resource.
  bool enemy_is_to_left = false;
  bool enemy_is_to_centre = false;
  bool enemy_is_to_right = false;

  int distance_ultrasonic_left_cm = ultrasonic_left.read();  // It is better to call this every time to ensure we have most up to date data.
  int distance_ultrasonic_centre_cm = ultrasonic_centre.read();  // we should check for 0 < ring diameter here as some things return negative if nothing detected
  int distance_ultrasonic_right_cm = ultrasonic_right.read();  // YOU MIGHT have to constantly update these in the loop

  int old_distance_ultrasonic_left_cm = distance_ultrasonic_left_cm;
  int old_distance_ultrasonic_centre_cm = distance_ultrasonic_centre_cm;
  int old_distance_ultrasonic_right_cm = distance_ultrasonic_right_cm;

  bool front_left_ir_value = digitalRead(FRONT_LEFT_IR);  // YOU MIGHT have to constantly update these in the loop
  bool back_left_ir_value = digitalRead(BACK_LEFT_IR);
  bool back_right_ir_value = digitalRead(BACK_RIGHT_IR);
  bool front_right_ir_value = digitalRead(FRONT_RIGHT_IR);

  unsigned long turnStartTime;

  // Starting 'wiggle' to find enemy bot.
  turnStartTime = millis();
  while (millis() - turnStartTime < 100 && !(enemy_is_to_left || enemy_is_to_centre || enemy_is_to_right)) {
    forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
    update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
  }
  turnStartTime = millis();
  while (millis() - turnStartTime < 200 && !(enemy_is_to_left || enemy_is_to_centre || enemy_is_to_right)) {
    forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
    update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
  }
  turnStartTime = millis();
  while (millis() - turnStartTime < 100 && !(enemy_is_to_left || enemy_is_to_centre || enemy_is_to_right)) {
    forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
    update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
  }


  // Main loop
  // I CAN PROBABLY GET RID OF THE CHECKS THAT CHECK IF ITS GREATER THAN 0 BECAUSE APPARENTLY NOW THEY REUTRN A VERY BIG VALUE 200000 IF NOTHING IS DETECTED.
  while (true) {

    update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);

    // Edge avoidance system
    // // Cases where enemy is far away
    // if (!enemy_is_to_left && !enemy_is_to_right && !enemy_is_to_centre) {
    //   // Cases where one sensor is triggered
    //   if (front_left_ir_value == 0 && back_left_ir_value != 0 && back_right_ir_value != 0 && front_right_ir_value != 0) {
    //     turnStartTime = millis();
    //     while (millis() - turnStartTime < 500) {
    //       reverse_turn(motorA, motorB, CLOCKWISE, MAX_SPEED / 10);
    //       update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
    //     }
    //     turnStartTime = millis();
    //     while (millis() - turnStartTime < 500) {
    //       forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);
    //       update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
    //     }

    //   } else if (front_left_ir_value != 0 && back_left_ir_value == 0 && back_right_ir_value != 0 && front_right_ir_value != 0) {
    //     turnStartTime = millis();
    //     while (millis() - turnStartTime < 500) {
    //       forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED / 3);
    //       update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
    //     }

    //   } else if (front_left_ir_value != 0 && back_left_ir_value != 0 && back_right_ir_value == 0 && front_right_ir_value != 0) {
    //     turnStartTime = millis();
    //     while (millis() - turnStartTime < 500) {
    //       forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / 3);
    //       update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
    //     }

    //   } else if (front_left_ir_value != 0 && back_left_ir_value != 0 && back_right_ir_value != 0 && front_right_ir_value == 0) {
    //     turnStartTime = millis();
    //     while (millis() - turnStartTime < 500) {
    //       reverse_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / 10);
    //       update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
    //     }

    //     turnStartTime = millis();
    //     while (millis() - turnStartTime < 500) {
    //       forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED);
    //       update_distance_and_direction_readings(&enemy_was_to_left, &enemy_was_to_centre, &enemy_was_to_right, &enemy_is_to_left, &enemy_is_to_centre, &enemy_is_to_right, &distance_ultrasonic_left_cm, &distance_ultrasonic_centre_cm, &distance_ultrasonic_right_cm, &old_distance_ultrasonic_left_cm, &old_distance_ultrasonic_centre_cm, &old_distance_ultrasonic_right_cm, &front_left_ir_value, &back_left_ir_value, &back_right_ir_value, &front_right_ir_value);
    //     }
    //   }
    //     // Cases where two sensors are triggered

    //     // Cases where more then two sensors are trigged (should never happen but just in case)
    // // Cases where enemy is right next or in front of us
    // } else {
    //   // Cases where one sensor is triggered

    //   // Cases where two sensors are triggered

    //   // Cases where more then two sensors are trigged (should never happen but just in case)
    // }





    // Main attack/movement/search system - THESE VALUES IN THE TURNING ESPECIALLY NEED TWEAKING - TURN SLOWER MAYBE?????
    // Check current statuses - priority is given to these and we go off these first.
    if (enemy_is_to_centre) {
      // Go forward
      driveStraight(motorA, motorB, MAX_SPEED);
    } else if (enemy_is_to_left) {
      // Turn left
      forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / max(distance_ultrasonic_left_cm / (RING_DIAMETER_CM / 2), 1));
    } else if (enemy_is_to_right) {
      // Turn right
      forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED / max(distance_ultrasonic_right_cm / (RING_DIAMETER_CM / 2), 1));

    // Check previous statuses. We go off these as a second resource of information if the enemy is not directly in front of our sensors at this moment in time.
    // These might need some fine tuning - same with the primary resource variants.
    } else if (enemy_was_to_centre) {
      // Go forward
      driveStraight(motorA, motorB, MAX_SPEED);
    } else if (enemy_was_to_left) {
      // Turn left
      forward_turn(motorA, motorB, ANTICLOCKWISE, MAX_SPEED / max(old_distance_ultrasonic_left_cm / (RING_DIAMETER_CM / 2), 1));
    } else if (enemy_was_to_right) {
      // Turn right
      forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED / max(old_distance_ultrasonic_right_cm / (RING_DIAMETER_CM / 2), 1));

    } else {
      // Search by spinning on the spot -- MIGHT CHANGE TO SPIN SLOWER??? OR MAYBE NOT COS AT THE START WE COULD BE DOOMED. IMPLEMENT STRAIGHT LINE CHARGE STRATEGY, but THAT MEANS THEY WILL BE ABLE TO INTERCEPT OR TRACK US IF WE PUT OUR ROBOTS TO THE SAME SIDE.? 
      // OR MAYBE WE PURPOSEFULLY PLACE IT SO THAT OUR SIDEWAYS ULTRASONIC SENSOR ALWAYS COVERS THE POSSIBLE SPOTS THEY COULD PLACE THEIR BOTS AT!!!
      // forward_turn(motorA, motorB, CLOCKWISE, MAX_SPEED);

      // Or here is the charge in straight line strategy to close off the potential spaces our opponent can be in.
      driveStraight(motorA, motorB, MAX_SPEED);
    }
  }
}

void test_ultrasonics() {
  int distance_ultrasonic_left_cm = ultrasonic_left.read();  // It is better to call this every time to ensure we have most up to date data.
  int distance_ultrasonic_centre_cm = ultrasonic_centre.read();  // we should check for 0 < ring diameter here as some things return negative if nothing detected
  int distance_ultrasonic_right_cm = ultrasonic_right.read();  // YOU MIGHT have to constantly update these in the loop

  // Example: Sending data to Serial Monitor
  Serial.print("Left value: ");
  Serial.println(distance_ultrasonic_left_cm);
  Serial.print("Centre value: ");
  Serial.println(distance_ultrasonic_centre_cm);
  Serial.print("Right value: ");
  Serial.println(distance_ultrasonic_right_cm);
}

void loop() {
  Serial.println("begin");
  // main_competition_strategy();
  testing_movement();
  // test_ultrasonics();
  delay(1000);
}
