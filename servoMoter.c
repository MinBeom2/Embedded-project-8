//소프트웨어적으로 pwm 생성 -> 내부적으로 gpio를 thread로 제어, 따라서 컴파일시 -lpthread
//gcc -o test test.c -lwiringPi -lpthread

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>

#define gpio_pins_num 3
int gpio_pins[gpio_pins_num] = {17, 18, 27} //softpwm 설정용 gpio 핀 배열

void softPwmControl(int gpio)
{
    pinMode(gpio, OUTPUT);         //Pin의 출력설정
    softPwmCreate(gpio, 0, 200);   //range 200 
}

void rotate_Servo(int pwm_pin, float angle) {
    float duty = 15 + (angle / 90.0) * 5;
    
    softPwmWrite(pwm_pin, pulse); //pulse는 10 ~ 20값(5% ~ 10%), 1ms ~ 2ms
}

int main()
{
    wiringPiSetupGpio( );  //wiringPi 초기화

    //softpwm 설정
    for(int i=0;i < gpio_pins_num;i++){
        softPwmControl(gpio_pins[i]);
    }

    int servo, angle;

    while(1){
        printf("서보모터(1,2,3)와 각도를 입력하시오. \n");
        scanf("%d %d", &servo, &angle)

        rotate_Servo(gpio_pins[servo], angle);
        delay(500);  
    };

    return 0;
}