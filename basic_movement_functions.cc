/*
	Basic Movement Functions
*/

// Speeds
MAX_SPEED = 255;
MIN_SPEED = 0;

// Directions
ANTICLOCKWISE = -1;
FORWARD = 0;
CLOCKWISE = 1;

// WE WILL HAVE TO CHANGE THESE VALUES FOR OUR ROBOT.
// Motor A
int enA = 9;
int inA_1 = 8;
int inA_2 = 7;

// Motor B
int enB = 3;
int inB_1 = 5;
int inB_2 = 4;

void setup() {
    // Set all the motor control pins to outputs using pinMode()
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(inA_1, OUTPUT);
    pinMode(inA_2, OUTPUT);
    pinMode(inB_1, OUTPUT);
    pinMode(inB_2, OUTPUT);
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

void turn()
