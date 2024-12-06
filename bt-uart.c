#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include <wiringSerial.h>
#include <stdint.h>  

#define BAUD_RATE 115200
#define GPIO1 18

static const char* UART2_DEV = "/dev/ttyAMA2"; // UART2
unsigned char serialRead(const int fd); 
void serialWrite(const int fd, const unsigned char c); 

unsigned char serialRead(const int fd){
    unsigned char x;
    if(read(fd, &x, 1) != 1) 
        return -1;
    return x; // 읽어온 데이터 반환
}

void serialWrite(const int fd, const unsigned char c){
    write(fd, &c, 1); 
}

// msg 전달 rasp->phone
void *Send(void *arg){
    unsigned char msg;
    int fd = (int)(intptr_t)arg; 
    while(1){
        scanf("%c", &msg);
        serialWrite(fd, msg);
        printf("%c", msg);
        fflush(stdout);
    }
    return NULL; 
}

int main(){
    pthread_t ptSend;
    int fd_serial;
    char dat;
    char buffer[100];
    int index = 0;

    if (wiringPiSetupGpio() < 0) return 1;
    if ((fd_serial = serialOpen(UART2_DEV, BAUD_RATE)) < 0) { // UART2 포트 오픈
        printf("Unable to open serial device.\n");
        return 1;
    }

    pthread_create(&ptSend, NULL, Send, (void *)(intptr_t)fd_serial);

    while(1){
        if(serialDataAvail(fd_serial)){ // 읽을 데이터가 존재한다면
            dat = serialRead(fd_serial); 
            fflush(stdout);
            printf("%c",dat);

            if(dat == '\n' || dat == '\r'){
                buffer[index] = '\0';
                if (strcmp(buffer, "1234") == 0){ //특정 조건 충족시 함수 수행
                    printf("SUCC\n");
                }
                memset(buffer, '\0', sizeof(buffer));
                index = 0;
            }else{
                buffer[index] = dat;
                index++;
            }
        }

        delay(10);
    }

    pthread_join(ptSend, NULL); 
}
