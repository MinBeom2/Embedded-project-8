#include <wiringPi.h>
#include <stdio.h>

#define PWM0 13

// Step GPIO
// IN1(12) ~ IN4(21)
int pin_arr[4] = {12, 16, 20, 21};

int one_phase[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
};

void one_two_Phase_Rotate(int steps, int dir)
{
    if (dir == 1)
    {
        for (int i = 0; i < steps; i++)
        {
            digitalWrite(pin_arr[0], one_phase[i % 8][0]);
            digitalWrite(pin_arr[1], one_phase[i % 8][1]);
            digitalWrite(pin_arr[2], one_phase[i % 8][2]);
            digitalWrite(pin_arr[3], one_phase[i % 8][3]);
            delay(2);
        }
    }
    else
    {
        for (int i = 0; i < steps; i++)
        {
            digitalWrite(pin_arr[0], one_phase[i % 8][3]);
            digitalWrite(pin_arr[1], one_phase[i % 8][2]);
            digitalWrite(pin_arr[2], one_phase[i % 8][1]);
            digitalWrite(pin_arr[3], one_phase[i % 8][0]);
            delay(2);
        }
    }
}

void one_two_Phase_Rotate_Angle(float angle, int dir)
{
    // 4096 이 한바퀴 1-2상 기준
    int steps = angle * (4096 / 360);

    one_two_Phase_Rotate(steps, dir);
}

void init_Step()
{
    // 4
    for (int i = 0; i < 4; i++)
    {
        pinMode(pin_arr[i], OUTPUT);
    }
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//서보모터


void rotate_Servo(float angle){
    // printf("%d", angle);
    float duty = 150 + (angle / 90) * 100;  //2.5% 7.5% 12.5%

    pwmWrite(PWM0, (int)duty);
}


int main()
{
    // printf("Stepping motor example\n");
    wiringPiSetupGpio(); /* wiringPi */
    
    //스텝모터 gpio 설정
    init_Step();

    //서보모터 설정
    pinMode(PWM0, PWM_OUTPUT); 
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(192);
    pwmSetRange(2000);
    delay(100);

    //서보모터 초기화
    pwmWrite(PWM0, 150);


    int moterSelect = 0;

    int angle, dir;
    while (1)
    {
        printf("모터 선택: ");
        scanf("%d", &moterSelect);
        
        if(moterSelect == 0){
            printf("angle: ");
            scanf("%d", &angle);

            if(angle <= 90 && angle >= -90){
                rotate_Servo((float)angle);
            }
        }else if(moterSelect == 1){
            printf("angle direction: ");
            scanf("%d %d", &angle, &dir);
            one_two_Phase_Rotate_Angle(angle, dir);
        }
    }

    return 0;
}
