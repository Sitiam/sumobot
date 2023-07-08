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

#include <Ultrasonic.h> 

// Pin assignments
const int ultrasonicTrigPin = 10;   // Ultrasonic sensor trigger pin
const int ultrasonicEchoPin = 11;   // Ultrasonic sensor echo pin
const int infraredSensorPin = A0;   // Infrared sensor analog pin

// Create instances of the Ultrasonic class and the Infrared sensor
Ultrasonic ultrasonicSensor(ultrasonicTrigPin, ultrasonicEchoPin);

int readUltrasonicSensor() {
	unsigned int distance = ultrasonicSensor.read();  // Read the distance from the ultrasonic sensor in centimeters
	return distance;
}

int readInfraredSensor() {
	// Since we are using arduino Nano, analogRead here will return an integer value between 0 and 1023 (10 bits). (idk what the infraredsensorpin actually returns someone search it up)
	int sensorValue = analogRead(infraredSensorPin);  // Read the analog value from the infrared sensor
	return sensorValue;
}

