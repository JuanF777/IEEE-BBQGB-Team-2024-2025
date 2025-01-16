/***************************************************************************************************************************
 *                                         Raspberry Pi 4B GPIO Pinout Breakdown
 * 
 *                                 Physical Pin | GPIO Number | Function                  | Notes
 * -------------------------------------------------------------------------------------------------------------------------
 *                   Left Column (Odd Pins)                   |                Right Column (Even Pins)
 * -------------------------------------------------------------------------------------------------------------------------
 *  1   | -       | 3.3V Power           | Power Output       |  2   | -       | 5V Power            | Power Output
 *  3   | GPIO2   | I2C1 SDA             | I2C Data           |  4   | -       | 5V Power            | Power Output
 *  5   | GPIO3   | I2C1 SCL             | I2C Clock          |  6   | -       | GND                 | Ground
 *  7   | GPIO4   | GPIO General Purpose | Default: GPCLK0    |  8   | GPIO14  | UART0 TXD           | Transmit Pin
 *  9   | -       | GND                  | Ground             | 10   | GPIO15  | UART0 RXD           | Receive Pin
 * 11   | GPIO17  | GPIO General Purpose | Default: GPIO      | 12   | GPIO18  | PWM0/PCM_CLK        | PWM/Audio Clock
 * 13   | GPIO27  | GPIO General Purpose | Default: GPIO      | 14   | -       | GND                 | Ground
 * 15   | GPIO22  | GPIO General Purpose | Default: GPIO      | 16   | GPIO23  | GPIO General Purpose| Default: GPIO
 * 17   | -       | 3.3V Power           | Power Output       | 18   | GPIO24  | GPIO General Purpose| Default: GPIO
 * 19   | GPIO10  | SPI0 MOSI            | SPI Master Out     | 20   | -       | GND                 | Ground
 * 21   | GPIO9   | SPI0 MISO            | SPI Master In      | 22   | GPIO25  | GPIO General Purpose| Default: GPIO
 * 23   | GPIO11  | SPI0 SCLK            | SPI Clock          | 24   | GPIO8   | SPI0 CE0            | SPI Chip Enable 0
 * 25   | -       | GND                  | Ground             | 26   | GPIO7   | SPI0 CE1            | SPI Chip Enable 1
 * 27   | GPIO0   | ID EEPROM SDA        | ID EEPROM (I2C)    | 28   | GPIO1   | ID EEPROM SCL       | ID EEPROM (I2C)
 * 29   | GPIO5   | GPIO General Purpose | Default: GPIO      | 30   | -       | GND                 | Ground
 * 31   | GPIO6   | GPIO General Purpose | Default: GPIO      | 32   | GPIO12  | PWM0                | Default: GPIO
 * 33   | GPIO13  | PWM1                 | Default: GPIO      | 34   | -       | GND                 | Ground
 * 35   | GPIO19  | PWM1/PCM_FS          | PWM/Audio Sync     | 36   | GPIO16  | GPIO General Purpose| Default: GPIO
 * 37   | GPIO26  | GPIO General Purpose | Default: GPIO      | 38   | GPIO20  | PCM_DIN             | Audio Data In
 * 39   | -       | GND                  | Ground             | 40   | GPIO21  | PCM_DOUT            | Audio Data Out
 ***************************************************************************************************************************/

#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <math.h>

int enableA = 23; // pin 16
// the pwm for each motor should be on the same channel 
// left motor: channel 0 
// PWMxy | x: channel, y: PWM driver number
int PWM01 = 12; // pin 32
int PWM02 = 18; // pin 12
//right motor: channel 1
int PWM11 = 13; // pin 33
int PWM12 = 19; // pin 35

int initialize() {
    if (gpioInitialise() < 0)
    {
        //initialization failed 
        printf(stderr, "Failed to initialize pigpio\n");
        return 1; 
    } 
    // set PWM frequencies 
    gpioSetPWMfrequency(PWM1, 1000);
    gpioSetPWMfrequency(PWM2, 1000);

    // set pin modes 
    gpioSetMode(enableA, PI_OUTPUT); 
    gpioSetMode(PWM1, PI_OUTPUT);
    gpioSetMode(PWM2, PI_OUTPUT);

    //set initial states 
    gpioPWM(PWM01, 0); 
    gpioPWM(PWM02, 0);
    gpioPWM(PWM11, 0); 
    gpioPWM(PWM12, 0); 
    gpioWrite(enableA, 0); 

}

int move_lr_motor(char motor, int pos, int op) {
    //in-between function to avoid making two copies of move_drive_motor 
    // possibly a better solution 
    // move left motor
    if (motor == 'l') {
        move_drive_motor(PWM01, PWM02, pos, op);
    }
    else if (motor == 'r') {
        move_drive_motor(PWM01, PWM02, pos, op);
    }
    else {
        printf("Not a valid motor, specify left (l) or right (r)");
        return 1;
    }
}

int move_drive_motor(int PWM1, int PWM2, int position, int operation) {
    // PWM1, PWM2: designate the PWM pins from the Pi to motor driver
    // position: PWM% out of 100 
    // operation: input should be one of the integer values below, 
    // which perform a certain action on the motor
    // -2 | reverse/brake at speed PWM 
    // -1 | brake low
    //  0 | coast
    //  1 | forward/brake at speed PWM
    if (position < 0 || position > 100) {
        printf("Position out of range\n");
        return 1; 
    }
    position = (255*position / 100); 

    // reverse/brake at speed PWM
    if (operation == -2) {
        //ENA = 1, PWM1 = 0, PWM2 = PWM
        gpioWrite(enableA, 1); 
        gpioWrite(PWM1, 0);
        gpioPWM(PWM2, position);
    }
    //brake low 
    else if (operation == -1) {
        //ENA = 1, PWM1 = 0, PWM2 = 0
        gpioWrite(enableA, 1); 
        gpioWrite(PWM1, 0);
        gpioWrite(PWM2, 0);
    }
    //coast
    else if (operation == 0) {
        //ENA = 0, PWM1 = 0, PWM2 = 0
        gpioWrite(enableA, 0); 
        gpioWrite(PWM1, 0);
        gpioWrite(PWM2, 0);
    }
    //forward/brake at speed PWM
    else if (operation == 1) {
        //ENA = 1, PWM1 = PWM, PWM2 = 0
        gpioWrite(enableA, 1); 
        gpioPWM(PWM1, position);
        gpioWrite(PWM2, 0);
    }
}

int main() {

    initialize(); 

    // test left motor
    move_lr_motor('l', 50, 1);
    sleep(1); 
    move_lr_motor('l', 50, 0); 
    sleep(1);
    move_lr_motor('l', 50, -1);
    sleep(1);
    move_lr_motor('l', 50, -2);
    sleep(1);
    move_lr_motor('l', 50, 0); //good to disable motor before exiting code
    gpioTerminate();
    return 0;
    
}