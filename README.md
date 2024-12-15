# Smart Pill Dispenser

## 1. 개요

 본 프로젝트에서는 처방과 다른 방식으로 약 복용 즉, 오남용을 했을 때의 부작용 발생을 최소화 하기 위해 여러 센서 및 액추에이터들을 활용하여 오남용 발생 위험을 낮추고 약 관리에 도움되는 ***Smart Pill Dispenser***를 개발하였다.    


    

      

   
  ![전체 사진](https://github.com/user-attachments/assets/099fd972-8083-42e5-92d6-d2c1979634e5)

 

 ## 2. 아이디어 소개

**<관리자>**
- 버튼과 블루투스 통신으로 서보모터를 제어하여 디스펜서 잠금 기능
- 잠금상태 및 비밀번호 입력 메시지 전송

**<사용자>**
- NFC와 블루투스 통신으로 사용자 확인 및 경고 메시지 전송
- 스텝 모터를 이용한 약 배출
- 약 복용 조건 미출족시 부저에서 경고음 출력 및 경고 메시지 전송

## 3. 하드웨어 및 회로도
![액추에이터 정리](https://github.com/user-attachments/assets/f59fe71a-4fad-4ca2-9b95-6e01e30a9c20)


<img width="913" alt="KakaoTalk_20241209_235023376_01" src="https://github.com/user-attachments/assets/88028db3-0f01-4ecf-b1a3-294e92fbb3c8" />

## 4. 전체 시스템 구조

![시스템 구조](https://github.com/user-attachments/assets/c2943308-21a5-4749-ac8c-2882677fa64f)


## 5. 시스템 흐름도

![main함수](https://github.com/user-attachments/assets/2e59f94d-43cf-49da-aefe-37c1f0c7b0c2)
![핵심함수](https://github.com/user-attachments/assets/2ca05113-6647-4b9b-82be-dc7649a7dd76)


## 6. 제한 조건 구현 내용
![제한조건 구현내용](https://github.com/user-attachments/assets/f23f62e8-25e1-4d2b-9141-97da4c8a44e8)


## 7. 개발 시 문제점 및 해결 방안

**<문제점>**
1. 우드락의 강도가 약해 축이 우드락을 갈아서 모터가 헛도는 문제 발생
2. UART로 장치(휴대폰)에 buffer전송 시 이전에 보낸 문자열이 겹쳐서 나타나는 현상 발생
3. UART핀과 SPI핀이 충돌되어 NFC와 블루투스가 작동하지 않는 현상 발생

**<해결 방안>**
1. 캔에서 자른 알루미늄 겹쳐서 스텝모터 축에 붙인 후 우드락에 부착하여 해결
2. buffer의 인덱스 초기화하고 buffer 내용을 memset으로 초기화하여 문제 해결
3. UART 핀의 개수를 줄이고 기존 /dev/ttyAMA2에 연결되어 있던 장치를 /dev/ttyAMA0으로 연결하여 문제 해결
## 8. 유저 사용 설명서

- Serial Bluetooth Terminal 앱 실행
- Bluetooth 모듈 연결
- ![장치 연결](https://github.com/user-attachments/assets/fed9204e-c045-4006-870f-5c01c90926a7)



**<약 관리>**
- 버튼을 누름
- 휴대폰 블루투스 터미널에서 비밀번호 입력
- 디스펜서 잠금 해제 
- 약 추가 및 제거
- 버튼을 다시 눌러 잠금

**<약 복용>**
- NFC 키를 NFC 리더기에 태그
- 휴대폰 블루투스 터미널에서 비밀번호 입력
- 약 복용 조건 충족시 스탭모터를 통해 약 배출
- 스텝 모터를 이용한 약 배출

**<약 복용 조건>**
- 하루 복용량 n회 초과하는지
- 마지막 약 복용 후 시간이 약 복용 간격 m 보다 큰지
- 약 복용 조건 미충족시 부저에서 경고음 출력

**<약 복용 횟수 초기화>** 
- 하루가 지나면 하루 복용량 n이 0으로 초기화됨

## 9. 데모 영상

https://www.youtube.com/shorts/A7KJ-okTqQo

       
**<순서>**
1. 버튼을 누르고 휴대폰으로 관리자 비밀번호 입력해 잠금 해제
2. 알약 보충 후 버튼을 눌러 잠금
3. NFC태그 후 사용자 비밀번호를 입력하여 스탭 모터 가동하여 약 복용
4. 복용 간격 및 복용 횟수 휴대폰으로 확인 가능
5. 약 복용 조건(횟수, 간격) 미충족시 부저 울리고 휴대폰에 메시지 전송
6. 하루가 지나면 약 복용횟수가 초기화됨
7. 만약 관리자나 사용자 비밀번호를 틀리게 입력시 비밀번호 입력 메시지 전송

