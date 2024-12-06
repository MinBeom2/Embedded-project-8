#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include <wiringSerial.h>
#include <stdint.h>  // intptr_t 정의를 위한 헤더 추가

#define BAUD_RATE 115200
#define GPIO1 18

static const char* UART2_DEV = "/dev/ttyAMA2"; // UART2 (RPi5는 UART1)연결을 위한 장치 파일
unsigned char serialRead(const int fd); // 1Byte 데이터를 수신하는 함수
void serialWrite(const int fd, const unsigned char c); // 1Byte 데이터를 송신하는 함수

// 1Byte 데이터를 수신
unsigned char serialRead(const int fd){
    unsigned char x;
    if(read(fd, &x, 1) != 1) // read 함수를 통해 1바이트 읽어옴
        return -1;
    return x; // 읽어온 데이터 반환
}

// 1Byte 데이터를 송신
void serialWrite(const int fd, const unsigned char c){
    write(fd, &c, 1); // write 함수를 통해 1바이트 씀
}

// msg 전달
void *Send(void *arg){
    unsigned char msg;
    // void *를 intptr_t로 변환 후 다시 int로 변환
    int fd = (int)(intptr_t)arg; 
    while(1){
        scanf("%c", &msg);
        serialWrite(fd, msg);
        printf("%c", msg);
        fflush(stdout);
    }
    return NULL; // return NULL로 종료
}

int main(){
    pthread_t ptSend;
    int fd_serial;
    char dat;

    if (wiringPiSetupGpio() < 0) return 1;
    if ((fd_serial = serialOpen(UART2_DEV, BAUD_RATE)) < 0) { // UART2 포트 오픈
        printf("Unable to open serial device.\n");
        return 1;
    }

    // fd_serial을 intptr_t로 변환 후 void *로 전달
    pthread_create(&ptSend, NULL, Send, (void *)(intptr_t)fd_serial);

    while(1){
        if(serialDataAvail(fd_serial)){ // 읽을 데이터가 존재한다면
            dat = serialRead(fd_serial); // 버퍼에서 1바이트 값을 읽음
            fflush(stdout);
            printf("%c",dat);
        }
        delay(10);
    }

    pthread_join(ptSend, NULL); // 스레드 종료 대기
}
