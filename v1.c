int findPower(float gyroVelocity, int motorVelocity){
  int power = 0;
  if (gyroVelocity > 700)
  {
    power = -65;
    return power;
  }
  else if (gyroVelocity < -700)
  {
    power = 65;
  }



}





task main(){

}
