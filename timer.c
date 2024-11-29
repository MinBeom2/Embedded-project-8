#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// 아래 4개 uart설정
int min_time = 0;
int max_time = 0;
int first_time = 0; // 시,분 설정해야할듯?
int max_count = 0;  // 하루 복용횟수

int today_count = 0;
int total_count = 0;      // 필요한가...?
int latest_take_time = 0; // 최근복용시간 근데 필요한가...? thread로 해결되는듯?

int flag = 0; // 공유 자원 약 복용 가능 여부 1: 가능 0: 불가능
pthread_mutex_t mid;

void *take_min(void *arg)
{
    sleep(min_time);            // 최소 시간 대기
    pthread_mutex_lock(&mid);   // 뮤텍스 잠금
    flag = 1;                   // 최소 시간이 지나서 먹을 수 있음
    pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
    printf("알림take min\n");
    return NULL;
}

void *take_max(void *arg)
{
    sleep(max_time); // 최대 시간 대기

    pthread_mutex_lock(&mid);
    if (flag == 1)
    {
        printf("알림take max\n");
        // 알림 발생 함수 실행
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
        if (current_time->tm_hour == first_time - 1 && flag == 1) // 첫 복용 시간에서 일정시간 (1시간) 전에 flag 활성화
        {
            pthread_mutex_lock(&mid);
            flag = 1;
            pthread_mutex_unlock(&mid);
            printf("하루의 시작, 복용 가능");
        }

        if (current_time->tm_hour == first_time && flag == 1) // 첫 복용 시간 마지노 선 입갤
        {
            printf("첫 복용시간 마지노선 알림");
            // 알림 함수
        }
        pthread_mutex_unlock(&mid);
        sleep(60); // 1분마다 확인
    }
    return NULL;
}

void timer()
{
    pthread_t min_thread, max_thread, first_thread;

    pthread_mutex_lock(&mid);
    if (flag == 1)
    {
        // latest_take_time = 현재시간

        pthread_mutex_lock(&mid); // 뮤텍스 잠금
        flag = 0;
        pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
        // 모터 작동
        today_count++;
        total_count++; // 복용 횟수 추가

        if (today_count < max_count) // 하루 복용횟수 안 채움
        {
            pthread_create(&min_thread, NULL, take_min, NULL);
            pthread_create(&max_thread, NULL, take_max, NULL);

            pthread_detach(min_thread);
            pthread_detach(max_thread); // 자동 회수
        }

        else // 하루 복용횟수 채움
        {
            today_count = 0;                                       // 하루 복용 횟수 다 채워서 초기화
            pthread_create(&first_thread, NULL, first_take, NULL); // 첫 복용 쓰레드 시작
            pthread_detach(first_thread);
        }
    }
    pthread_mutex_unlock(&mid);

    return 0;
}

int main()
{
    wiringPiSetupGpio();
    pthread_mutex_init(&mid, NULL);

    while (1)
    {
        // if 태그 성공
        timer();
        delay(200); // 0.2s대기 (과부하 방지)
    }

    pthread_mutex_destroy(&mid);
    return 0;
}