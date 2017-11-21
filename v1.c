typedef struct
{
	float degrees;
	float rate;
	int motorPort;
	int sensorPort;
	int flip;
} axis;



int findPower(float gyroVelocity)
{
	int power = 0;
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
		power = gyroVelocity / 10.034;
	}
	return power;
}

/*bool failSafe(float gDegrees)
{
	if (fabs(gDegrees)>500)
		return false;
	return true;
}*/

float taperDist(int motorPower)
{
	int direction = 0;
	if(motorPower < 0)
		direction = -1;
	else if(motorPower > 0)
		direction = 1;

	float sumDegreesLeft = 0;
	const float deltaT = 0.001;
	for(int i = abs(motorPower); i > 0; i--)
	{
		sumDegreesLeft += 10.034 * i * deltaT;
	}
	return direction * sumDegreesLeft;
}

void taper(int motorPower, int motorPort, int timerNUM)
{
	if(motorPower > 0)
	{
		for( ; motorPower > 0; motorPower--)
		{
			motor[motorPort] = motorPower;
			while(time1[timerNUM] < 1)
			{}
		}
	}
	else if(motorPower < 0)
	{
		for( ; motorPower < 0; motorPower++)
		{
			motor[motorPort] = motorPower;
			while(time1[timerNUM] < 1)
			{}
		}
	}
}



task main()
{

	setLEDColor(ledRedPulse);

	axis pitch;
		pitch.degrees = 0;
		pitch.rate = 0;
		pitch.motorPort = motorA;
		pitch.sensorPort = S1;
		pitch.flip = -1;

	axis roll;
		roll.degrees = 0;
		roll.rate = 0;
		roll.motorPort = motorB;
		roll.sensorPort = S2;
		roll.flip = -1;

	axis yaw;
		yaw.degrees = 0;
		yaw.rate = 0;
		yaw.motorPort = motorC;
		yaw.sensorPort = S3;
		yaw.flip = 1;

/////////////////////////////////////////////////////
	SensorType[pitch.sensorPort] = sensorEV3_Gyro;
	SensorType[roll.sensorPort] = sensorEV3_Gyro;
	SensorType[yaw.sensorPort] = sensorEV3_Gyro;

	SensorType[S4] = sensorEV3_Touch;

	wait1Msec(1000);
	resetGyro(pitch.sensorPort);
	resetGyro(roll.sensorPort);
	resetGyro(yaw.sensorPort);

	wait1Msec(1000);
	nMotorEncoder[pitch.motorPort] = 0;
	nMotorEncoder[roll.motorPort] = 0;
	nMotorEncoder[yaw.motorPort] = 0;

	playSound(soundBeepBeep);
	setLEDColor(ledGreenPulse);

	while(SensorValue(S4) == 0)
	{}
	while(SensorValue(S4) == 1)
	{}

	setLEDColor(ledOrangePulse);

	while(1)
	{
		displayCenteredTextLine(3, "Sensor Value: %d", getGyroDegrees(S1));
		displayCenteredTextLine(4, "Sensor Value: %d", getGyroDegrees(S2));

		pitch.degrees = getGyroDegrees(pitch.sensorPort);
		pitch.rate = getGyroRate(pitch.sensorPort);

		roll.degrees = getGyroDegrees(roll.sensorPort);
		roll.rate = getGyroRate(roll.sensorPort);

		yaw.degrees = getGyroDegrees(yaw.sensorPort);
		yaw.rate = getGyroRate(yaw.sensorPort);

		motor[pitch.motorPort] = 5*findPower(pitch.rate)*pitch.flip;
		motor[roll.motorPort] = 5*findPower(roll.rate)*roll.flip;
		motor[yaw.motorPort] = 5*findPower(yaw.rate);

/*		while(motor[pitch.motorPort] != 0)
		{
			displayCenteredTextLine(5, "%f", (nMotorEncoder[pitch.motorPort] - taperDist(motor[pitch.motorPort])));

			if(((float)nMotorEncoder[pitch.motorPort] - taperDist(motor[pitch.motorPort])) > pitch.degrees)
			{
				taper(motor[pitch.motorPort], pitch.motorPort, T1);
			}
		}*/


	}
}
