/*
  변박사 [A3-2안]

  상태1 {
    조도센서를 통한 변기 착석 행위 감지 중…
  }

  상태2 {
    조도센서를 통한 변기 착석 행위 감지 시, 상태2 진입

    “박수는 엉덩이를 춤추게 한다” 음성 출력(음원파일1 실행)
    조도센서를 통한 기립 행위 감지 중…
  }

  상태3-1 {
    조도센서를 통한 기립 행위 감지 시, 상태3 진입
  
    “이야-” 음성 출력(음원파일2 실행), 양팔(왼팔&오른팔) 아래 동작 동시수행
      왼팔: 모터2 시계방향으로 90도 회전(바깥시점 기준)
      오른팔: 모터2 반시계방향으로 90도 회전(바깥시점 기준)

    모터2 동작 종료 후 1초 경과 시, 양팔(왼팔&오른팔) 아래 동작 5초간 동시수행
      박수 및 함성 소리 음원 동시 출력(음원파일3 실행)
      왼팔:
        모터1 반시계방향으로 15도 회전(위시점 기준)
        모터1 시계방향으로 15도 회전(위시점 기준)
      오른팔:
        모터1 시계방향으로 15도 회전(위시점 기준)
        모터1 반시계방향으로 15도 회전(위시점 기준)

    모터1 동작 종료 후 1초 경과 시, 양팔(왼팔&오른팔) 아래 동작 동시수행
      왼팔: 모터2 반시계방향으로 90도 회전(바깥시점 기준)
      오른팔: 모터2 시계방향으로 90도 회전(바깥시점 기준)
    
      모터2 동작 종료 후 1초 경과 시, 상태1 진입
  }

  상태3-2 {
    스위치를 통한 버튼눌림 감지 시, 양팔(왼팔&오른팔) 아래 동작 동시수행
      “이야-” 음성 출력(음원파일2 실행), 양팔(왼팔&오른팔) 아래 동작 동시수행
        왼팔: 모터2 시계방향으로 90도 회전(바깥시점 기준)
        오른팔: 모터2 반시계방향으로 90도 회전(바깥시점 기준)
      
    모터2 동작 종료 후 1초 경과 시, 양팔(왼팔&오른팔) 아래 동작 5초간 동시수행
      박수 및 함성 소리 음원 출력(음원파일3 실행)
      왼팔:
        모터1 반시계방향으로 15도 회전(위시점 기준)
        모터1 시계방향으로 15도 회전(위시점 기준)
      오른팔:
        모터1 시계방향으로 15도 회전(위시점 기준)
        모터1 반시계방향으로 15도 회전(위시점 기준)
    
    모터1 동작 종료 후 1초 경과 시, 양팔(왼팔&오른팔) 아래 동작 동시수행
      왼팔: 모터2 반시계방향으로 90도 회전(바깥시점 기준)
      오른팔: 모터2 시계방향으로 90도 회전(바깥시점 기준)

    모터2 동작 종료 후 1초 경과 시, 상태1 진입
  }
*/


#include <Servo.h>

// 조도센서, 버튼 및 서보모터 연결 핀
const int PIN_LDR  = A0;
const int PIN_BTN  = 2;
const int PIN_L_M1 = 9;
const int PIN_L_M2 = 6;
const int PIN_R_M1 = 3;
const int PIN_R_M2 = 5;

// 조도 감지 설정 (두 값 사이는 경계값으로 무시)
const int DARK_TH   = 300;
const int BRIGHT_TH = 500;

// 착석 및 기립 판정 확정 시간 설정
const unsigned long SIT_CONFIRM_MS   = 1000;
const unsigned long STAND_CONFIRM_MS = 1000;

// 버튼 채터링(접점 떨림) 무시 시간
const unsigned long BTN_DEBOUNCE_MS = 50;


// 서보 각도 설정 (서보 각도가 커질수록 반시계방향으로 회전)
// 모터1 (어깨 들어오기, 바깥시점 기준)
const int L_M1_REST = 0;
const int L_M1_UP   = 90;
const int R_M1_REST = 180;
const int R_M1_UP   = 90;
// 모터2 (팔꿈치 박수, 위시점 기준)
const int M2_CENTER = 90;
const int L_M2_CLAP = M2_CENTER - 180;
const int R_M2_CLAP = M2_CENTER + 180;

// 타이밍
const int ARM_STEP_MS         = 8;     // 팔 올리고 내릴 때 1도당 지연(작을수록 빠름)
const unsigned long WAIT_1S   = 1000;
const unsigned long CLAP_TIME = 8000;
const int CLAP_HALF_MS        = 150;   // 박수 반동작(15도 갔다 오기 중 한쪽) 시간


// 상태 설정
enum State { IDLE, SEATED, CLAPPING };
State state = IDLE;

Servo leftM1, leftM2, rightM1, rightM2;

// 조도 감지 조건이 일정 시간 충족했는지 확인하기 위한 값 설정
bool condActive = false;
unsigned long condStart = 0;

bool heldFor(bool cond, unsigned long ms) {
  if (!cond) {
    condActive = false;
    return false;
  }
  if (!condActive) {
    condActive = true;
    condStart = millis();
  }
  return (millis() - condStart >= ms);
}

void resetHold() {
  condActive = false;
}

// 버튼이 '새로 눌린 순간'에만 true (누르고 있는 동안 계속 true 아님)
int btnStable = HIGH;
int btnLastRead = HIGH;
unsigned long btnChangedAt = 0;

bool buttonPressed() {
  int reading = digitalRead(PIN_BTN);
  if (reading != btnLastRead) {
    btnLastRead = reading;
    btnChangedAt = millis();
  }
  if (millis() - btnChangedAt >= BTN_DEBOUNCE_MS && reading != btnStable) {
    btnStable = reading;
    if (btnStable == LOW) return true;
  }
  return false;
}

// 두 서보를 동시에 같은 속도로 이동 (양팔 동시수행)
void moveTogether(Servo &a, int aFrom, int aTo, Servo &b, int bFrom, int bTo, int stepMs) {
  int steps = max(abs(aTo - aFrom), abs(bTo - bFrom));
  for (int i = 1; i <= steps; i++) {
    a.write(aFrom + (long)(aTo - aFrom) * i / steps);
    b.write(bFrom + (long)(bTo - bFrom) * i / steps);
    delay(stepMs);
  }
}

// 노트북 음원 재생 호출
void requestSound(int fileNo) {
  Serial.print("PLAY");
  Serial.println(fileNo);   // "PLAY1", "PLAY2", "PLAY3"
}

// 상태3(CLAPPING) 시퀀스 (3-1, 3-2 공통)
void runClapSequence() {

  // ① 음원2 재생 요청 + 양팔 들기 (모터2)
  requestSound(2);
  Serial.println(F("LOG: 팔 올리기"));
  moveTogether(leftM1, L_M1_REST, L_M1_UP, rightM1, R_M1_REST, R_M1_UP, ARM_STEP_MS);
  delay(WAIT_1S);

  // ② 음원3 재생 요청 + 5초간 왔다리갔다리 박수 (모터1)
  requestSound(3);
  Serial.println(F("LOG: 박수 시작"));
  unsigned long clapStart = millis();
  while (millis() - clapStart < CLAP_TIME) {
    // 왼팔 반시계 15도, 오른팔 시계 15도
    leftM2.write(L_M2_CLAP);
    rightM2.write(R_M2_CLAP);
    delay(CLAP_HALF_MS);

    // 왼팔 시계 15도, 오른팔 반시계 15도 (원위치)
    leftM2.write(M2_CENTER);
    rightM2.write(M2_CENTER);
    delay(CLAP_HALF_MS);
  }
  delay(WAIT_1S);

  // ③ 양팔 내리기 (모터2 원위치)
  Serial.println(F("LOG: 팔 내리기"));
  moveTogether(leftM1, L_M1_UP, L_M1_REST, rightM1, R_M1_UP, R_M1_REST, ARM_STEP_MS);
  delay(WAIT_1S);
}



// ================== setup / loop ==================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_BTN, INPUT_PULLUP);

  leftM1.attach(PIN_L_M1);
  leftM2.attach(PIN_L_M2);
  rightM1.attach(PIN_R_M1);
  rightM2.attach(PIN_R_M2);

  // 초기 자세: 팔 내리고 숨긴 상태
  leftM2.write(M2_CENTER);
  rightM2.write(M2_CENTER);
  leftM1.write(L_M1_REST);
  rightM1.write(R_M1_REST);
  delay(500);

  Serial.println(F("LOG: 변박사 시작 - 상태1(IDLE)"));
}

void loop() {
  int ldr = analogRead(PIN_LDR);
  bool pressed = buttonPressed();   // 버튼 상태는 매 loop마다 갱신해야 함

  // 0.5초마다 센서값 출력 (임계값 조정용)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print(F("LOG:LDR="));
    Serial.print(ldr);
    Serial.print(F(" state="));
    Serial.println(state + 1);   // 1, 2, 3
  }

  // 상태3-2: 조도센서 고장 대비 안전장치 (상태1, 상태2 어디서든 버튼 누르면 바로 박수)
  if (pressed && state != CLAPPING) {
    resetHold();
    Serial.println(F("LOG: 버튼 눌림 -> 상태3-2(CLAPPING)"));
    state = CLAPPING;
  }

  switch (state) {
    case IDLE:       // 상태1: 착석 감지 중
      if (heldFor(ldr < DARK_TH, SIT_CONFIRM_MS)) {
        resetHold();
        Serial.println(F("LOG: 착석 감지 -> 상태2(SEATED)"));
        requestSound(1);
        state = SEATED;
      }
      break;

    case SEATED:     // 상태2: 기립 감지 중
      if (heldFor(ldr > BRIGHT_TH, STAND_CONFIRM_MS)) {
        resetHold();
        Serial.println(F("LOG: 기립 감지 -> 상태3-1(CLAPPING)"));
        state = CLAPPING;
      }
      break;

    case CLAPPING:   // 상태3: 음원2 + 팔 올리기 → 음원3 + 박수 → 팔 내리기
      runClapSequence();
      resetHold();
      Serial.println(F("LOG: 시퀀스 종료 -> 상태1(IDLE)"));
      state = IDLE;
      break;
  }
}
