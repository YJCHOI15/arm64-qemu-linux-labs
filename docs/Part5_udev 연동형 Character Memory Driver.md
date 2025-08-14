# 커널 모듈 기반 Charmem 디바이스 드라이버 구현

## 목표 기능
디바이스 노드(`/dev/charmem`)를 udev와 연동하여 만든다.

1. 주번호는 자동으로 할당, 부번호는 0번
2. 전역변수로 4KB의 `char` 배열을 선언하고 문자 데이터를 저장
3. **write** 시 전역 배열의 내용을 버퍼 내용으로 덮어씀  
   - 요청 크기가 배열 크기보다 크면, 배열 크기까지만 복사  
   - 성공한 바이트 수를 반환
4. **read** 시 전역 배열 내용을 버퍼로 복사  
   - 요청 크기가 배열 크기보다 크면, 배열 크기까지만 복사  
   - 성공한 바이트 수를 반환

---

## Loadable Kernel Module (LKM)
- 커널 이미지에 포함되지 않으면서 커널 기능을 확장할 수 있는 바이너리
- 커널 재부팅 없이 기능 확장 가능
- 부팅 시간과 메모리 사용량 절약 가능

---

## 디바이스 노드
- **타입**: 문자형(c), 블록형(b)
- **주번호(Major)**: 디바이스 드라이버를 구분하는 번호 (0~511)
- **부번호(Minor)**: 디바이스를 구분하는 번호 (0~1048576)
- 동일 드라이버로 여러 디바이스 관리 가능
- `/dev` 아래에서 생성 및 관리

---

## udev 동작 방식
1. 커널이 uevent를 발생 → udev 데몬에 전달
2. udev 데몬이 `/sys`를 확인해 디바이스 정보를 수집  
   - `/sys/devices`, `/sys/class`에 디렉토리 생성  
   - 디렉토리 내 `dev` 파일을 읽어 주/부번호 확인
3. udev 데몬이 `/dev` 아래에 디바이스 노드를 생성

---

## 구현 과정

### 1. 드라이버 디렉토리 생성
```bash
mkdir /linux/drivers/charmem
```

### 2. /linux/drivers/Makefile 수정
makefile
Copy
Edit
obj-y += charmem/
### 3. /linux/drivers/Kconfig 수정
makefile
Copy
Edit
source "drivers/charmem/Kconfig"
endmenu
### 4. /linux/drivers/charmem/Kconfig 작성
makefile
Copy
Edit
menu "Charmem Module Configuration"

config CHARMEM_MODULE
    tristate "Charmem Module"
    help
      This is a kernel module for charmem device driver.

endmenu
### 5. /linux/drivers/charmem/Makefile 작성
makefile
Copy
Edit
obj-$(CONFIG_CHARMEM_MODULE) += charmem.o
charmem-objs += main.o
### 6. /linux/drivers/charmem/main.c 작성
드라이버 메인 소스 구현

### 7. menuconfig 설정
bash
Copy
Edit
ARCH=arm64 make menuconfig
경로: Device Drivers → Charmem Module Configuration → <M> Charmem Module

### 8. 커널 빌드
bash
Copy
Edit
ARCH=arm64 CROSS_COMPILE=<툴체인>/bin/aarch64-none-linux-gnu- make -j$(nproc)
### 9. Rootfs에 .ko 복사
bash
Copy
Edit
sudo mount -o loop <buildroot>/output/images/rootfs.ext4 /mnt
sudo mkdir -p /mnt/usr/lib/modules
sudo cp drivers/charmem/charmem.ko /mnt/usr/lib/modules
sync
sudo umount /mnt
### 10. 사용자 프로그램 작성/배포
c
Copy
Edit
// user_charmem.c — 간단한 읽기/쓰기 예제
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    int fd = open("/dev/charmem_buffer", O_WRONLY);
    write(fd, "Hello Charmem", 13);
    close(fd);

    char buf[4096] = {0};
    fd = open("/dev/charmem_buffer", O_RDONLY);
    int r = read(fd, buf, sizeof(buf));
    write(STDOUT_FILENO, buf, r);
    close(fd);
    return 0;
}
빌드 및 배포:

bash
Copy
Edit
<툴체인>/bin/aarch64-none-linux-gnu-gcc -o user_charmem user_charmem.c

sudo mount -o loop buildroot/output/images/rootfs.ext4 /mnt
sudo cp user_charmem /mnt/usr/bin
sync
sudo umount /mnt
### 11. QEMU 실행 및 테스트
bash
Copy
Edit
qemu-system-aarch64 \
  -kernel <linux>/arch/arm64/boot/Image \
  -drive format=raw,file=<buildroot>/output/images/rootfs.ext4,if=virtio \
  -append "root=/dev/vda console=ttyAMA0 nokaslr" \
  -nographic -M virt -cpu cortex-a72 \
  -m 2G -smp 2
### 12. 동작 확인
insmod charmem.ko 실행 → "charmem" 클래스 생성 확인

/dev/charmem_buffer 디바이스 자동 생성(udev 동작)

사용자 프로그램 실행 시 문자열이 정상적으로 저장/출력됨

