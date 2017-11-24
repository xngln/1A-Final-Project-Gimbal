//Display + AngleFinder

//gyroAngle = angle where gyro is
//nmotorencoder = angle where we are for that axis

float findAngle(float gyroAngle, float nmotorencoder)
{
	float angleDiff = 0;

	if(nmotorencoder < gyroAngle)
	{
	angleDiff = gyroAngle - nmotorencoder;
	}
	else if(gyroAngle < nmotorencoder)
	{
		angleDiff = nmotorencoder - gyroAngle;
	}

	return angleDiff;
}

void displayValues(float gyroAngle, float nmotorEncoder)
{
	bool progOn = true;
	while(progOn == true)
	{
		displayBigTextLine(3,"gyroAngle = %.2f, nmotorEncoder = %.2f",gyroAngle,nmotorEnconder);
	}
}

//If we're putting it into
task main()
{
	angleDiff = findAngle(float gyroAngle, float nmotorencoder)

}
