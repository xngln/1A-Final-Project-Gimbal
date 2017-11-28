typedef struct
{
	float degrees;
	float rate;
	int motorPort;
	int sensorPort;
	int flip;
	int gearRatio;
	float error;
} axis;

int findPower(int port, float gyroVelocity, char MotorSize)
{
	int power = 0;
	float ratio = 10.034;
	if (MotorSize == 'M')
	{
		ratio = 15.897;
	}

	if (gyroVelocity > 700)
	{
			power = -65;
	}
	else if (gyroVelocity < -700)
	{
			power = 65;
	}
	else
	{
		power = gyroVelocity / ratio;
	}

	return power;
}

float findEncoderGyroDifference(axis Axis)
{
	float angDiff = 0;
	if (Axis.flip > 0)
		angDiff = getGyroDegrees(Axis.motorPort) - nMotorEncoder[Axis.motorPort]/Axis.gearRatio;
	else
		angDiff = -getGyroDegrees(Axis.motorPort) - nMotorEncoder[Axis.motorPort]/Axis.gearRatio;
	return angDiff;
}

bool buttonPress(int & displaymode, int touchSensorPort)
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

bool failSafe()
{
	if (fabs(getGyroDegrees(S1)) > 100 || fabs(getGyroDegrees(S2)) > 85 || fabs(getGyroDegrees(S3)) > 95)
		return true;
	else
		return false;
}

void displayVars(int mode)
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

void tuneKs(float & kp, float & kr, float & ky, float increment)
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
//																																		 //
//																																		 //
/////////////////////////////////////////////////////////////////////////
task main()
{

	setLEDColor(ledRedPulse);

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

	axis yaw;
	yaw.degrees = 0;
	yaw.rate = 0;
	yaw.motorPort = motorC;
	yaw.sensorPort = S3;
	yaw.flip = -1;
	yaw.gearRatio = 5;

	/////////////////////////////////////////////////////
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

		motor[pitch.motorPort] = pitch.gearRatio*findPower(1,pitch.rate, 'L')*pitch.flip   +   kp*pitch.error;
		motor[roll.motorPort] = roll.gearRatio*findPower(2,roll.rate, 'L')*roll.flip  +   kr*roll.error;
		motor[yaw.motorPort] = yaw.gearRatio*findPower(3,yaw.rate, 'M')*yaw.flip   +   ky*yaw.error;

		pitch.degrees = getGyroDegrees(pitch.sensorPort);
		roll.degrees = getGyroDegrees(roll.sensorPort);
		yaw.degrees = getGyroDegrees(yaw.sensorPort);

		if(fabs(pitch.degrees - nMotorEncoder[pitch.motorPort]/pitch.gearRatio) > 1)
		{
			pitch.error = findEncoderGyroDifference(pitch);
		}
		if(fabs(roll.degrees - nMotorEncoder[roll.motorPort]/roll.gearRatio) > 1)
		{
			roll.error = findEncoderGyroDifference(roll);
		}
		if(fabs(yaw.degrees - nMotorEncoder[yaw.motorPort]/yaw.gearRatio) > 1)
		{
			yaw.error = findEncoderGyroDifference(yaw);
		}
	}
}
