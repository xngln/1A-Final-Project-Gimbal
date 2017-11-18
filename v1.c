int findPower(float gyroVelocity, int motorVelocity)
{
  int power = 0;
  if (gyroVelocity > 700)
  {
    power = -65;
    return power;
  }
  else if (gyroVelocity < -700)
  {
    power = 65;
    return power;
  }
  else if (gyroVelocity > 0)
  {
    power = gyroVelocity / -10.034;
    return power;
  }
  else
  {
    power = gyroVelocity / -10.034;
    return power;
  }
}





task main(){

}
