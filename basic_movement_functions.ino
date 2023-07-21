/*
	Basic Functions

	NOTES:
	- delay(1000) makes the robot wait for 1000 milliseconds.
	- maybe we end up using async functions if they exist in C++?
  
  TODO:
  - Sensor input.
	- Strategy implementation.
*/

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

void loop() {
	testing_movement();
}
