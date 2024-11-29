#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
#include <stdlib.h>

void music(int gpio)
{
    softToneCreate(gpio);
    for (int i = 0, i < 3; i++)
    {
        softToneWrite(gpio, 450);
        delay(250);
    }
}

int main(int argc, char **argv)
{
    int gno;
    if (argc < 2)
    {
        printf("Usage : %s Enter the duty\n", argv[0]);
        return -1;
    }
    wiringPiSetupGpio();
    gno = atoi(argv[1]);
    musicPlay(gno);
    return 0;
}
