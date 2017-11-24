
bool failSafe(axis axis)
{
	if (abs(getGyroRate(axis.sensorPort))>800|| abs(getGyroDegrees(axis.sensorPort))> axis.angle)
	{
		time1[T1] = 0;
		while (time1[T1]<2000||abs(getGyroRate(axis.sensorPort))>800|| abs(getGyroDegrees(axis.sensorPort))> axis.angle)
		{
			setLEDColor(ledRedPulse);
			setSoundVolume(100);
			playSoundFile("beep");
		}
		return 0;
	}

}
