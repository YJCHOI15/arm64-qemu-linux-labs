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
커널 소스 트리 내에서 독립된 디렉토리로 관리하면, 빌드 설정과 유지보수가 쉬워진다. 
```bash
mkdir /linux/drivers/charmem
```

### 2. /linux/drivers/Makefile 수정
커널 빌드 시스템이 `drivers/charmem` 디렉토리를 인식하고 빌드에 포함하도록 한다.
```makefile
obj-y += charmem/
```

### 3. /linux/drivers/Kconfig 수정
`make menuconfig`에서 charmem 모듈을 설정할 수 있게 메뉴 항목을 추가한다. 
```makefile
...
source "drivers/charmem/Kconfig"
endmenu
```

### 4. /linux/drivers/charmem/Kconfig 작성
모듈을 동적으로 로드/언로드 할 수 있도록 `tristate` 설정을 추가한다. 

### 5. /linux/drivers/charmem/Makefile 작성
`CONFIG_CHARMEM_MODULE`이 y나 m으로 설정됐을 때만 빌드하도록 한다. 

### 6. /linux/drivers/charmem/main.c 작성
드라이버 메인 소스 구현

### 7. menuconfig 설정
```bash
ARCH=arm64 make menuconfig
```

Device Drivers → Charmem Module Configuration → <M> Charmem Module 선택
모듈을 LKM으로 빌드하도록 선택한다. 

### 8. 커널 빌드
```bash
ARCH=arm64 CROSS_COMPILE=<툴체인>/bin/aarch64-none-linux-gnu- make -j$(nproc)
```

### 9. Rootfs에 .ko 복사
빌드한 모듈을 타겟 환경에 배포하여 런타임에 `insmod`로 로드할 수 있게 한다. 
```bash
sudo mount -o loop <buildroot>/output/images/rootfs.ext4 /mnt
sudo mkdir -p /mnt/usr/lib/modules
sudo cp drivers/charmem/charmem.ko /mnt/usr/lib/modules
sync
sudo umount /mnt
```

### 10. 사용자 프로그램 작성/배포
`user_charmem.c` 작성

`/dev/charmem_buffer` 디바이스를 직접 열어서 write()로 쓰고 read()로 읽는 간단한 사용자 공간 예제이다. 

툴체인을 통해 빌드 후 rootfs에 프로그램 배포

```bash
<툴체인>/bin/aarch64-none-linux-gnu-gcc -o user_charmem user_charmem.c

sudo mount -o loop buildroot/output/images/rootfs.ext4 /mnt
sudo cp user_charmem /mnt/usr/bin
sync  
sudo umount /mnt
```

### 11. QEMU 실행 및 테스트
```bash
qemu-system-aarch64 \
  -kernel <linux>/arch/arm64/boot/Image \
  -drive format=raw,file=<buildroot>/output/images/rootfs.ext4,if=virtio \
  -append "root=/dev/vda console=ttyAMA0 nokaslr" \
  -nographic -M virt -cpu cortex-a72 \
  -m 2G -smp 2
```

### 12. 동작 확인
insmod charmem.ko 실행 → "charmem" 클래스 생성 확인
<BR><img width="682" height="126" alt="image" src="https://github.com/user-attachments/assets/599db51e-29a2-4000-b847-8a848054e1a4" />


/dev/charmem_buffer 디바이스 자동 생성(udev 동작)
<BR><img width="542" height="70" alt="image" src="https://github.com/user-attachments/assets/3be4e69c-287e-4de1-9645-077a5c34d5f2" />

이처럼 udev 데몬이 `/sys/class/` 를 보고 있다가 아래와 같은 dev 파일을 읽어서 주번호와 부번호를 알아내고, `/dev` 아래에 해당 디바이스 노드를 생성하는 것이다. 
<BR><img width="282" height="41" alt="image" src="https://github.com/user-attachments/assets/c20be721-2c33-4957-8f63-96ffeb922ec2" />


사용자 프로그램 실행 시 문자열이 정상적으로 저장/출력됨
<BR><img width="178" height="66" alt="image" src="https://github.com/user-attachments/assets/d698a8ce-5bde-48ee-9a13-3c96adfb7a95" />

## 결론

이번 구현을 통해 커널 모듈(LKM) 형태의 문자 디바이스 드라이버를 작성하고, udev와 연동하여 /dev 노드가 자동 생성되는 과정을 경험했다.
자동 주번호 할당, 전역 버퍼 기반 read/write 처리, 스핀락을 이용한 동시성 제어까지 포함하여,
실제 하드웨어 장치 없이도 QEMU 환경에서 드라이버-사용자 공간 프로그램 간 데이터 교환을 검증할 수 있었다.

