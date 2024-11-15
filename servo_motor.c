#include <pigpio.h>

int servoPin = 12; 

int main() {
    //necessary initialization check 
    if (gpioInitialise() < 0)
    {
        //initialization failed
        printf(stderr, "Failed to initialize pigpio\n");
        return 1; 
    }

    //test 
    gpioServo(servoPin, 1500);
    sleep(1); 
    return 0; 
 //hello from Juan
}


/*gpioSetMode(servoPin, PI_OUTPUT);
gpioPWM(servoPin, 0);

int servo_motor(pin, position) 
{
     int duty = position*(7/100)+4;
     gpioSETPWMrange(pin, duty);

}

while True
{
    int
}*/
