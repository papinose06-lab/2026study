"""
변박사 [A3-2안] - 노트북 음원 재생 프로그램
"""

import os
import sys
import time

import pygame
import serial
import serial.tools.list_ports

BAUD = 9600
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# 음원 파일 이름 (확장자 없이). wav → mp3 → ogg 순서로 찾음
SOUND_NAMES = {
    "PLAY1": "sound1",
    "PLAY2": "sound2",
    "PLAY3": "sound3",
}


def find_sound_file(name):
    for ext in (".wav", ".mp3", ".ogg"):
        path = os.path.join(BASE_DIR, name + ext)
        if os.path.exists(path):
            return path
    return None


def load_sounds():
    pygame.mixer.init()
    pygame.mixer.set_num_channels(8)  # 음원2와 음원3이 겹쳐도 같이 재생되도록
    sounds = {}
    for cmd, name in SOUND_NAMES.items():
        path = find_sound_file(name)
        if path is None:
            print(f"[경고] {name}.wav/.mp3/.ogg 파일이 없습니다 → {cmd}는 무시됩니다")
            continue
        sounds[cmd] = pygame.mixer.Sound(path)
        print(f"[준비] {cmd} → {os.path.basename(path)}")
    return sounds


def find_arduino_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        desc = f"{p.description} {p.manufacturer or ''}".lower()
        if any(k in desc for k in ("arduino", "ch340", "usb serial", "usb-serial", "wch")):
            return p.device
    # 못 찾으면 usbmodem/ttyACM/ttyUSB 이름으로 한 번 더
    for p in ports:
        if any(k in p.device for k in ("usbmodem", "usbserial", "ttyACM", "ttyUSB")):
            return p.device
    print("[오류] 아두이노 포트를 찾지 못했습니다. 연결된 포트 목록:")
    for p in ports:
        print(f"   {p.device}  ({p.description})")
    return None


def open_serial(port):
    ser = serial.Serial(port, BAUD, timeout=1)
    time.sleep(2)  # 포트를 열면 아두이노가 리셋되므로 부팅 대기
    ser.reset_input_buffer()
    return ser


def main():
    sounds = load_sounds()

    port = sys.argv[1] if len(sys.argv) > 1 else find_arduino_port()
    if port is None:
        sys.exit(1)

    print(f"[연결] {port} @ {BAUD}bps  (종료: Ctrl+C)")
    ser = open_serial(port)

    try:
        while True:
            try:
                raw = ser.readline()
            except serial.SerialException:
                # USB가 빠졌을 때: 다시 꽂힐 때까지 재연결 시도
                print("[끊김] 아두이노 연결이 끊겼습니다. 재연결 시도 중...")
                ser.close()
                while True:
                    time.sleep(2)
                    try:
                        ser = open_serial(port)
                        print("[연결] 재연결 완료")
                        break
                    except serial.SerialException:
                        pass
                continue

            if not raw:
                continue

            line = raw.decode("utf-8", errors="replace").strip()
            if not line:
                continue

            if line in sounds:
                sounds[line].play()
                print(f"♪ {line} 재생")
            elif line.startswith("LOG"):
                # 센서값 로그(LOG:LDR=...)가 너무 많으면 아래 두 줄 주석 처리
                print(line)
            else:
                print(f"[알 수 없는 줄] {line}")

    except KeyboardInterrupt:
        print("\n[종료]")
    finally:
        ser.close()
        pygame.mixer.quit()


if __name__ == "__main__":
    main()