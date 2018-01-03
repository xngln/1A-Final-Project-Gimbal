//
typedef struct 
{
	float degrees;
	float rate;
	int motorPort;
	int sensorPort;
	int flip; // takes into account the orientation of the gyro relative to the corresponding motor
	int gearRatio;
	float error;
} axis;


/* Returns a power that such that the motor moves at a velocity equal in magnitude and opposite in direction to gyro.
   Ratio was determined experimentally.
*/
int findPower(int port, float gyroVelocity, char MotorSize) // by Xianglin Liu
{
	int power = 0;
	float ratio = 10.034;
	if (MotorSize == 'M')
		ratio = 15.897;

	if (gyroVelocity > 700)
		power = -65;
	
	else if (gyroVelocity < -700)
		power = 65;
	else
		power = gyroVelocity / ratio;

	return power;
}


// finds the difference in angle between a motor and the corresponding gyro
float findEncoderGyroDifference(axis Axis) // by Yu Yuan Chang
{
	float angDiff = 0;
	if (Axis.flip > 0) 
		angDiff = getGyroDegrees(Axis.motorPort) - nMotorEncoder[Axis.motorPort]/Axis.gearRatio;
	else
		angDiff = -getGyroDegrees(Axis.motorPort) - nMotorEncoder[Axis.motorPort]/Axis.gearRatio;
	return angDiff;
}


/* when touch sensor activated briefly, will increment displaymode variable
   if activated for greater than one second, program will end
*/
bool buttonPress(int & displaymode, int touchSensorPort) // by Rutvik Pandya
{
	bool endProg = false;
	if(SensorValue(touchSensorPort))
	{
		time1[T4] = 0;
		while(SensorValue(touchSensorPort) && time1[T4] <= 1000)
		{}
		if(time1[T4] >= 1000)
		{
			endProg = true;
		}
		else
		{
			displaymode++;
			if(displaymode > 3)
				displaymode = 1;
		}
	}
	return endProg;
}


// if device rotates more than the mechanical design allows or to an unsafe angle, function returns true
bool failSafe() // by Jeffrey Wu
{
	if (fabs(getGyroDegrees(S1)) > 100 || fabs(getGyroDegrees(S2)) > 85 || fabs(getGyroDegrees(S3)) > 95)
		return true;
	else
		return false;
}


// changes the display mode 
void displayVars(int mode) // by Xianglin Liu
{
	eraseDisplay();
	if (mode == 1)
	{
		displayBigTextLine(1, "PITCH");
		displayBigTextLine(5, "Gyro: %f.4", getGyroDegrees(S1));
		displayBigTextLine(9, "Encoder: %f.4", nMotorEncoder[motorA]/5);
	}
	else if (mode == 2)
	{
		displayBigTextLine(1, "ROLL");
		displayBigTextLine(5, "Gyro: %f.4", getGyroDegrees(S2));
		displayBigTextLine(9, "Encoder: %f.4", nMotorEncoder[motorB]/5);
	}
	else if (mode == 3)
	{
		displayBigTextLine(1, "YAW");
		displayBigTextLine(5, "Gyro: %f.4", getGyroDegrees(S3));
		displayBigTextLine(9, "Encoder: %f.4", nMotorEncoder[motorC]/5);
	}

}


// when a certain combination of buttons is pressed, the K values are incremented and the display is updated accordingly
void tuneKs(float & kp, float & kr, float & ky, float increment) // by Rutvik Pandya
{
	if(getButtonPress(buttonAny))
	{
		motor[motorA] = motor[motorB] = motor[motorC]= 0;
		while(getButtonPress(buttonEnter))
		{
			setLEDColor(ledRedPulse);
			displayBigTextLine(3, "kp: %f", kp);

			if (getButtonPress(buttonLeft))
			{
				while(getButtonPress(buttonLeft))
				{}
				kp -= increment;
			}
			if (getButtonPress(buttonRight))
			{
				while(getButtonPress(buttonRight))
				{}
				kp += increment;
			}
		}
		////////////////////////////////////////
		while(getButtonPress(buttonUp))
		{
			setLEDColor(ledRedPulse);
			displayBigTextLine(3, "kr: %f", kr);

			if (getButtonPress(buttonLeft))
			{
				while(getButtonPress(buttonLeft))
				{}
				kr -= increment;
			}
			if (getButtonPress(buttonRight))
			{
				while(getButtonPress(buttonRight))
				{}
				kr += increment;
			}
		}
		////////////////////////////////////////
		while(getButtonPress(buttonDown))
		{
			setLEDColor(ledRedPulse);
			displayBigTextLine(3, "ky: %f", ky);

			if (getButtonPress(buttonLeft))
			{
				while(getButtonPress(buttonLeft))
				{}
				ky -= increment;
			}
			if (getButtonPress(buttonRight))
			{
				while(getButtonPress(buttonRight))
				{}
				ky += increment;
			}
		}
	}
	setLEDColor(ledOrangePulse);
}


/////////////////////////////////////////////////////////////////////////
																																	 //
																																		 //
/////////////////////////////////////////////////////////////////////////

task main() // by Jeff Wu, Yu Yuan Chang
{

	setLEDColor(ledRedPulse);

	// intitializing structs for each axis
	axis pitch;
	pitch.degrees = 0;
	pitch.rate = 0;
	pitch.motorPort = motorA;
	pitch.sensorPort = S1;
	pitch.flip = -1;
	pitch.gearRatio = 5;
	pitch.error = 0;

	axis roll;
	roll.degrees = 0;
	roll.rate = 0;
	roll.motorPort = motorB;
	roll.sensorPort = S2;
	roll.flip = -1;
	roll.gearRatio = 5;
	roll.error = 0;

	axis yaw;
	yaw.degrees = 0;
	yaw.rate = 0;
	yaw.motorPort = motorC;
	yaw.sensorPort = S3;
	yaw.flip = -1;
	yaw.gearRatio = 5;
	yaw.error = 0;

	// initializing sensors
	SensorType[pitch.sensorPort] = sensorEV3_Gyro;
	SensorType[roll.sensorPort] = sensorEV3_Gyro;
	SensorType[yaw.sensorPort] = sensorEV3_Gyro;

	SensorType[S4] = sensorEV3_Touch;

	wait1Msec(1000);

	playSound(soundDownwardTones);

	setLEDColor(ledGreenPulse);
	resetGyro(pitch.sensorPort);
	resetGyro(roll.sensorPort);
	resetGyro(yaw.sensorPort);
	nMotorEncoder[pitch.motorPort] = 0;
	nMotorEncoder[roll.motorPort] = 0;
	nMotorEncoder[yaw.motorPort] = 0;
	wait1Msec(1000);

	// waits for user to press button connected to touch sensor
	while(SensorValue(S4) == 0)
	{}
	while(SensorValue(S4) == 1)
	{}

	setLEDColor(ledOrangePulse);

	float kp = 0.775;
	float kr = 0.9;
	float ky = 0.5;

	int displayMode = 1;
	bool endProg = false;

	while(!endProg && !failSafe())
	{
		displayVars(displayMode);
		tuneKs(kp, kr, ky, 0.001);
		endProg = buttonPress(displayMode, S4);

		pitch.rate = getGyroRate(pitch.sensorPort);
		roll.rate = getGyroRate(roll.sensorPort);
		yaw.rate = getGyroRate(yaw.sensorPort);

		// compensates for movement by assigning each motor a power proportional to gyro velocity
		motor[pitch.motorPort] = pitch.gearRatio*findPower(1,pitch.rate, 'L')*pitch.flip   +   kp*pitch.error;
		motor[roll.motorPort] = roll.gearRatio*findPower(2,roll.rate, 'L')*roll.flip  +   kr*roll.error;
		motor[yaw.motorPort] = yaw.gearRatio*findPower(3,yaw.rate, 'M')*yaw.flip   +   ky*yaw.error;

		pitch.degrees = getGyroDegrees(pitch.sensorPort);
		roll.degrees = getGyroDegrees(roll.sensorPort);
		yaw.degrees = getGyroDegrees(yaw.sensorPort);

		// finds error, or the difference between the motor encoder and the gyro position
		if(fabs(pitch.degrees - nMotorEncoder[pitch.motorPort]/pitch.gearRatio) > 1)
			pitch.error = findEncoderGyroDifference(pitch);

		if(fabs(roll.degrees - nMotorEncoder[roll.motorPort]/roll.gearRatio) > 1)
			roll.error = findEncoderGyroDifference(roll);
		
		if(fabs(yaw.degrees - nMotorEncoder[yaw.motorPort]/yaw.gearRatio) > 1)
			yaw.error = findEncoderGyroDifference(yaw);
	}
}
