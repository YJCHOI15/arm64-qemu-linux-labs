# Custom System Call: 32-bit Integer Array Access

ARM aarch64 기반 리눅스 커널에 사용자 정의 시스템 콜을 추가하고, QEMU 에뮬레이터에서 테스트하는 과정을 다룹니다. 
시스템 콜은 32비트 정수형 배열을 설정/조회하는 기능을 제공합니다.

---

## 기능 개요

- **set_array(uint32_t index, uint32_t value)**  
  지정한 인덱스에 정수 데이터를 저장

- **get_array(uint32_t index)**  
  지정한 인덱스에 저장된 정수 값을 반환

> 배열 크기: `256 / sizeof(uint32_t)` → 최대 64개의 요소

---

## 커널 시스템 콜 추가

### 1. 소스 파일 추가
`~/linux/kernel/array_syscalls.c`


### 2. 커널 빌드 설정
- `Makefile`에 `obj-y += array_syscalls.o` 추가

<img width="799" height="407" alt="image" src="https://github.com/user-attachments/assets/b8bbb34d-7a70-4c8d-9162-9b629cd82559" /> <br>
  
- `~/linux/arch/arm64/tools/syscall_64.tbl` 파일에 시스템콜을 추가 (커널 버전 6.11 이상)
- syscall 번호 할당 (`set_array` → 469, `get_array` → 470)

<img width="884" height="65" alt="image" src="https://github.com/user-attachments/assets/b49df0e1-d99f-4adc-bd47-7e8783a63e6b" />

---

## 리눅스 커널 빌드

```bash
ARCH=arm64 CROSS_COMPILE=<툴체인>/bin/aarch64-none-linux-gnu- make -j$(nproc)
```

<img width="1151" height="60" alt="image" src="https://github.com/user-attachments/assets/ee43b9ff-9b4d-4d5a-b1bb-9f5d5ec5d712" />


빌드 완료 후, `arch/arm64/boot/Image` 생성됨.

---

## 사용자 프로그램

`array_program.c` 작성

툴체인을 통해 타겟 아키텍쳐에 맞게 빌드:

```bash
<툴체인>/bin/aarch64-none-linux-gnu-gcc -o array_program array_program.c
```

정상 빌드 여부 확인:
<img width="1160" height="119" alt="image" src="https://github.com/user-attachments/assets/41c984ba-c0c6-4296-86fd-bce17fdc4c9f" />


---

## rootfs에 프로그램 배포

```bash
sudo mount -o loop buildroot/output/images/rootfs.ext4 /mnt
sudo cp array_program /mnt/usr/bin
sync # USB 안전하게 제거와 비슷하게 쓰기 후 적용이 완벽하게 보장되도록 함 
sudo umount /mnt
```

---

## QEMU 실행 테스트

```bash
qemu-system-aarch64 \
  -kernel <linux>/arch/arm64/boot/Image \
  -drive format=raw,file=<buildroot>/output/images/rootfs.ext4,if=virtio \
  -append "root=/dev/vda console=ttyAMA0 nokaslr" \
  -nographic -M virt -cpu cortex-a72 \
  -m 2G -smp 2
```

### 예시

<img width="463" height="95" alt="image" src="https://github.com/user-attachments/assets/7d7ee671-f818-4759-9455-fe6532507522" />


---

## 참고

- Toolchain: `aarch64-none-linux-gnu-`
- 테스트 환경: QEMU (ARM64 가상 머신)
- rootfs: Buildroot에서 생성한 `ext4` 이미지

---
