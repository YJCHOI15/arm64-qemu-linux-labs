# Custom System Call: 32-bit Integer Array Access

ARM aarch64 기반 리눅스 커널에 사용자 정의 시스템 콜을 추가하고, QEMU 에뮬레이터에서 테스트하는 과정을 다룬다. 
두 시스템 콜은 32비트 정수형 배열을 설정/조회하는 기능을 제공한다.

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
- `/linux/kernel/Makefile`에 `obj-y += array_syscalls.o` 추가
  - 이 오브젝트를 모듈(.ko)이 아닌 커널 본체(vmlinux)에 포함 시킴  
- `~/linux/arch/arm64/tools/syscall_64.tbl` 파일에 시스템콜을 추가 (커널 버전 6.11 이상)
  - syscall 번호 할당 (`set_array` → 470, `get_array` → 471)

---

## 리눅스 커널 빌드

`/linux` 에서 실행
```bash
ARCH=arm64 CROSS_COMPILE=<툴체인 디렉토리 절대경로>/bin/aarch64-none-linux-gnu- make -j$(nproc)
```
<img width="904" height="84" alt="image" src="https://github.com/user-attachments/assets/fe36e037-ff4e-41c1-ab31-a3b4f7521301" />


빌드 완료 후, `arch/arm64/boot/Image` 생성됨.

---

## 사용자 프로그램

`array_program.c` 작성

툴체인을 통해 타겟 아키텍쳐에 맞게 빌드:

```bash
<툴체인>/bin/aarch64-none-linux-gnu-gcc -o array_program array_program.c
```

정상 빌드 여부 확인:
<img width="906" height="113" alt="image" src="https://github.com/user-attachments/assets/576c9726-7cb5-48f2-95cf-ce615d39caa8" />

ARM aarch64에서 실행 가능한 파일임을 확인할 수 있다. 

---

## rootfs에 프로그램 배포

rootfs.ext4 안에 `array_program` 실행 파일을 넣어서, QEMU 부팅 환경에 반영한다.

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

<img width="921" height="113" alt="image" src="https://github.com/user-attachments/assets/b9bf11f1-db5a-428b-9f71-bb9e0cb9a356" />


### 예시

<img width="359" height="81" alt="image" src="https://github.com/user-attachments/assets/af928f50-ebd5-4634-9e33-1adad5f4f0c1" />


---

## 참고

- Toolchain: `aarch64-none-linux-gnu-`
- 테스트 환경: QEMU (ARM64 가상 머신)
- rootfs: Buildroot에서 생성한 `ext4` 이미지

---
