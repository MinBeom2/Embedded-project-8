#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
#include <stdlib.h>

void music(int gpio)
{
    softToneCreate(gpio);
    for (int i = 0; i < 3; i++)
    {
        softToneWrite(gpio, 900);
        delay(333);
        softToneWrite(gpio, 0);
        delay(333);
    }
}

int main(int argc, char **argv)
{

    wiringPiSetupGpio();
    music(18);
    return 0;
}