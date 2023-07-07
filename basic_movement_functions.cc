/*
	Basic Movement Functions

 	Sorry for the rambly comments but at least the code is still readable and self-documented.

 	NOTES:
	- delay(1000) makes the robot wait for 1000 milliseconds I think.
 	- maybe we end up using async functions if they exist in c++?
  
  	TODO:
   	- Input functions in a separate file
	- Strategy implementation in a separate file (main or something)
*/

// Speeds
int MAX_SPEED = 255;
int MIN_SPEED = 0;

// Directions
int ANTICLOCKWISE = -1;
int FORWARD = 0;
int CLOCKWISE = 1;

// Motor Constants (we will have to change these values for our robot)
// Motor A (Left Motor)
int enA = 9;
int inA1 = 8;
int inA2 = 7;

// Motor B (Right Motor)
int enB = 3;
int inB1 = 5;
int inB2 = 4;


struct motor {
	int en;
	int in1;
	int in2;
	int max_speed;
	int min_speed;
};

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
driveSingleWheel(motor selected_motor, int speed) {
	if (speed < 0) {
		digitalWrite(selected_motor.in1, LOW);
		digitalWrite(selected_motor.in2, HIGH);
		analogWrite(selector_motor.en, speed);
	} else {
		digitalWrite(selected_motor.in1, HIGH);
		digitalWrite(selected_motor.in2, LOW);
		analogWrite(selector_motor.en, speed);
	}
}

// Negative speed = backwards
void driveStraight(motor motorA, motor motorB, int speed) {
	driveSingleWheel(motorA, speed);
	driveSingleWheel(motorB, speed);
}

// void driveBackward(int speed) {
// 	digitalWrite(inA_1, LOW);
// 	digitalWrite(inA_2, HIGH);
// 	digitalWrite(inB_1, LOW);
// 	digitalWrite(inB_1, HIGH);
// 	analogWrite(enA, speed);  // Idk if this line is needed cos example code in slides doesn't have this
// 	analogWrite(enB, speed);  // Idk if this line is needed cos example code in slides doesn't have this
// }

void turnOffMotors() {
	digitalWrite(inA_1, LOW);
	digitalWrite(inA_2, LOW);
	digitalWrite(inB_1, LOW);
	digitalWrite(inB_1, LOW);
}

// Turns the robot while it moves forward.
// -1 wheel_pointing_direction will make the robot rotate anticlockwise if it is moving forwards. 0 will not make the wheels move. 1 will the the robot rotate clockwise.
// turn_speed should be an integer value between 0 and 100. 0 turn speed will not make the robot turn. 100 turn speed will make the robot turn very quickly on the spot.
// turn_speed in between will make it turn at a slower pace by making the robot still move forward as it turns. 
void forward_turn(int turn_direction, int turn_speed) {
	// // if (turn_speed == 0) {
	// // 	return
	// // }
	// if (turn_direction == -1) {
	// } (turn_direction == 0) {}

	
}

void reverse_turn() {
}

void main() {
	setup();
	
	// Set up the two motor structs
	motor motorA;
	motor motorB;
	motorA.en = enA;
	motorA.in1 = inA1;
	motorA.in2 = inA2;
	motorA.max_speed = MAX_SPEED;
	motorA.min_speed = MIN_SPEED;

	motorB.en = enA;
	motorB.in1 = inA1;
	motorB.in2 = inA2;
	motorB.max_speed = MAX_SPEED;
	motorB.min_speed = MIN_SPEED;
}

main();
