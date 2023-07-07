/*
	Basic Movement Functions
*/

// Speeds
MAX_SPEED = 255;
MIN_SPEED = 0;

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

void demoOne()
{
    // This function will run the motors in both directions at a fixed speed

    // Turn on motor A -- digitalWrite() sets a pin to either HIGH (5V) or LOW (0V)
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    // Set speed to 200 out of possible range 0~255 -- analogWrite() sends a PWM signal to the pin
    analogWrite(enA, 200);

    // Turn on motor B
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    // Set speed to 200 out of possible range 0~255
    analogWrite(enB, 200);
    // pauses the program for a given amount of time - 2s
    delay(2000);

    // Now change motor directions -- reversing the motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    delay(2000);

    // Now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

}

void demoTwo()
{

    // This function will run the motors across the range of possible speeds
    // Note that maximum speed is determined by the motor itself and the operating voltage

    // Turn on motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    // Accelerate from zero to maximum speed
    for (int i = 0; i < 256; i++)
    {

        analogWrite(enA, i);
        analogWrite(enB, i);

        delay(20);

    }

    // Decelerate from maximum speed to zero
    for (int i = 255; i >= 0; --i)
    {

        analogWrite(enA, i);
        analogWrite(enB, i);

        delay(20);

    }

    // Now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

}

void loop()
{

    demoOne();

    delay(1000);

    demoTwo();

    delay(1000);

}
