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

// WE WILL HAVE TO CHANGE THESE VALUES FOR OUR ROBOT.
// Motor A (Left Motor)
struct {
	int enA = 9;
	int inA_1 = 8;
	int inA_2 = 7;
} motorA;

// Motor B (Right Motor)
struct {
	int enB = 3;
	int inB_1 = 5;
	int inB_2 = 4;
} motorB;


void setup() {
	// Set all the motor control pins to outputs using pinMode()
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(inA_1, OUTPUT);
	pinMode(inA_2, OUTPUT);
	pinMode(inB_1, OUTPUT);
	pinMode(inB_2, OUTPUT);
}

// Speed between -MAX_SPEED (negative MAX_SPEED) and +MAX_SPEED (positive MAX_SPEED)
// Negative speed makes the motor spin backwards
driveSingleWheel(int motor_in_1, int motor_in_2, int en_motor, int speed) {
	digitalWrite(inA_1, HIGH);
	digitalWrite(inA_2, LOW);
}

void driveForward(int speed) {
	digitalWrite(inA_1, HIGH);
	digitalWrite(inA_2, LOW);
	digitalWrite(inB_1, HIGH);
	digitalWrite(inB_1, LOW);
	analogWrite(enA, speed);
	analogWrite(enB, speed);
}

void driveBackward(int speed) {
	digitalWrite(inA_1, LOW);
	digitalWrite(inA_2, HIGH);
	digitalWrite(inB_1, LOW);
	digitalWrite(inB_1, HIGH);
	analogWrite(enA, speed);  // Idk if this line is needed cos example code in slides doesn't have this
	analogWrite(enB, speed);  // Idk if this line is needed cos example code in slides doesn't have this
}

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
	// if (turn_speed == 0) {
	// 	return
	// }
	if (turn_direction == -1) {
		digitalWrite(inA_1, HIGH);
		digitalWrite(inA_2, LOW);
		digitalWrite(inB_1, HIGH);
		digitalWrite(inB_1, LOW);
	} (turn_direction == 0) {}
	digitalWrite(inA_1, HIGH);
	digitalWrite(inA_2, LOW);
	digitalWrite(inB_1, HIGH);
	digitalWrite(inB_1, LOW);
	
}

void reverse_turn() {
}
