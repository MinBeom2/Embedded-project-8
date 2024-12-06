#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include <wiringPi.h>
#include <softTone.h>

extern char **environ;

// 설정값들
int min_time = 10;  // 최소 대기 시간 (초 단위 예제용)
int max_time = 60;  // 최대 대기 시간 (초 단위 예제용)
int first_time = 9; // 첫 복용 시간 (24시간 형식)
int max_count = 3;  // 하루 복용 횟수 제한

// 상태 변수들
int today_count = 0;
int total_count = 0;
int flag = 0; // 공유 자원 약 복용 가능 여부 1: 가능 0: 불가능
pthread_mutex_t mid;

// NFC 태그 인식 결과 변수
char nfc_detected_data[100] = "";

// 스텝모터 gpio
// gpio 12, 16, 20, 21
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
//////////////////////////////////////////////////////////////

// 타이머 함수들
void *take_min(void *arg)
{
    sleep(min_time);
    pthread_mutex_lock(&mid);
    flag = 1;
    pthread_mutex_unlock(&mid);
    printf("알림: 최소 대기 시간 지나 복용 가능\n");
    return NULL;
}

void *take_max(void *arg)
{
    sleep(max_time);
    pthread_mutex_lock(&mid);
    if (flag == 1)
    {
        printf("알림: 최대 대기 시간 도달\n");
        music(18);
    }
    pthread_mutex_unlock(&mid);
    return NULL;
}

void *first_take(void *arg)
{
    while (1)
    {
        time_t now = time(NULL);
        struct tm *current_time = localtime(&now);
        pthread_mutex_lock(&mid);
        if (current_time->tm_sec == first_time - 1 && flag == 0)
        {
            flag = 1;
            printf("알림: 하루 첫 복용 가능\n");
        }
        if (current_time->tm_sec == first_time && flag == 0)
        {
            printf("알림: 하루 첫 복용 마감\n");
            music(18);
        }
        pthread_mutex_unlock(&mid);
        sleep(1);
    }
    return NULL;
}

void timer()
{
    pthread_t min_thread, max_thread, first_thread;
    pthread_mutex_lock(&mid);
    if (flag == 1)
    {
        flag = 0;

        // 스텝모터 작동(시계방향, 8칸 => 45도)
        one_two_Phase_Rotate_Angle(45, 0);

        today_count++;
        total_count++;
        printf("복용 완료, 오늘 복용 횟수: %d\n", today_count);
        if (today_count < max_count)
        {
            pthread_create(&min_thread, NULL, take_min, NULL);
            pthread_create(&max_thread, NULL, take_max, NULL);

            pthread_detach(min_thread);
            pthread_detach(max_thread);
        }
        else
        {
            today_count = 0;
            pthread_create(&first_thread, NULL, first_take, NULL);
            pthread_detach(first_thread);
        }
    }
    else
    {
        printf("하루 복용 횟수 초과");
        music(18);
    }
    pthread_mutex_unlock(&mid);
}

// NFC 감지 함수
int nfc_detect()
{
    pid_t pid;
    int status;
    char *argv[] = {"nfc-poll", NULL};
    char detected_data[100];

    printf("NFC 감지 중...\n");
    if (posix_spawn(&pid, "/bin/nfc-poll", NULL, NULL, argv, environ) != 0)
    {
        perror("nfc-poll 실행 실패");
        return 0;
    }

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("nfc-poll 대기 실패");
        return 0;
    }

    // 예제: NFC 데이터 수집 (여기선 시뮬레이션)
    FILE *file = popen("/bin/nfc-poll | grep 'UID' | awk '{print $3}'", "r");
    if (file)
    {
        fgets(detected_data, sizeof(detected_data), file);
        pclose(file);
        detected_data[strcspn(detected_data, "\n")] = '\0'; // 개행 제거
    }

    // 감지된 데이터 확인
    if (strlen(detected_data) > 0)
    {
        strcpy(nfc_detected_data, detected_data);
        printf("NFC 데이터 감지됨: %s\n", nfc_detected_data);
        return 1;
    }
    else
    {
        printf("NFC 태그를 감지하지 못했습니다.\n");
        return 0;
    }
}

int main()
{
    wiringPiSetupGpio();
    pthread_mutex_init(&mid, NULL);

    printf("프로그램 시작\n");

    // 스텝모터 gpio 설정
    init_Step();

    while (1)
    {
        if (nfc_detect())
        {
            printf("NFC 감지 성공: %s\n", nfc_detected_data);
            timer();
        }
        sleep(1); // CPU 과부하 방지
    }

    pthread_mutex_destroy(&mid);
    return 0;
}
