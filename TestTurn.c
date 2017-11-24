void turn(float degrees, int gearRatio, int motorPort)
{
	int initialEncoder = nMotorEncoder[motorPort];
	int tolerance = 3;
	motor[motorPort] = 40;
	while(fabs(degrees - fabs(nMotorEncoder[motorPort] - initialEncoder)*gearRatio) > tolerance)
	{}
	motor[motorPort] = 0;
}


task main()
{
	motor[motorB]=0;
	nMotorEncoder[motorB] = 0;
	while(SensorValue(S4) == 0)
	{}
	while(SensorValue(S4) == 1)
	{}

	turn(-10, motorB, 20);

	while(SensorValue(S4) == 0)
	{}
	while(SensorValue(S4) == 1)
	{}
	turn(-36, motorB, -20);
	while(SensorValue(S4) == 0)
	{}
	while(SensorValue(S4) == 1)
	{}
	turn(5, motorB, -20);
	while(SensorValue(S4) == 0)
	{}
	while(SensorValue(S4) == 1)
	{}
	turn(20, motorB, 100);
}
