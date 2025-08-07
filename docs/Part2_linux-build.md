리눅스 커널 빌드는 임베디드 시스템에서 커널을 사용자 정의하고 최적화하는 과정이다. 

툴체인을 사용해 커널을 컴파일하여 타겟 아키텍처에 맞는 커널 이미지를 생성한다. 

마지막으로 빌드한 리눅스 커널과 루트 파일 시스템으로 QEMU에서 리눅스를 실행한다. 

<BR>

# 1. 리눅스 커널 다운로드 및 종속성 패키지 설치

본 프로젝트에서는 가장 안정적인 버전인 stable linux를 사용한다. 

리눅스 커널을 빌드하는데 필요한 종속성 패키지들도 설치해준다. 

```bash
$ git clone https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/
$ sudo apt-get update
$ sudo apt-get install bison flex libelf-dev libssl-dev
```
<BR>

# 2. 툴체인 다운로드

참고로 리눅스 커널과 달리 빌드 루트는 자동으로 필요한 라이브러리와 툴체인을 다운받아 설치해준다. 

리눅스 커널을 빌드하기 위해 다음 사이트에서 .tar 형식의 툴체인을 다운받는다. 

https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

<BR>

x86-64 리눅스 호스트 환경에서 aarch64 리눅스 커널을 빌드하기 위한 버전이다. 

<img width="919" height="105" alt="image" src="https://github.com/user-attachments/assets/28c7eed2-e85e-42f8-b460-55c5430937bb" />


```bash
$ tar xvf <툴체인 tar 파일> /linux_ws
```
<BR>


# 3. QEMU에서 리눅스 커널 사용하기

Buildroot의 QEMU에서 리눅스 커널을 사용할 수 있도록 커널 설정 파일을 `qemu_defconfig` 라는 이름으로 복사한다. 

복사된 `qemu_defconfig` 파일은 커널 빌드를 QEMU용으로 최적화된 설정으로 실행할 수 있도록 도와준다. 

```bash
$ cp <buildroot 절대경로>/board/qemu/aarch64-virt/linux.config <linux 절대경로>/arch/arm64/configs/qemu_defconfig
```

<BR>

qemu_defconfig 파일을 사용하여, arm64 아키텍처를 타겟으로 하는 QEMU 환경에 최적화된 리눅스 커널 설정 파일(.config)을 생성한다. 

```bash
$ ARCH=arm64 make qemu_defconfig
```

<BR>

툴체인을 사용해 64비트 ARM용 커널을 빌드한다. 

물음표가 뜨면 계속 엔터를 눌러서 진행한다 (빌드루트에서 복사해온 설정 파일과 커널 버전을 맞춰준다).

```bash
$ ARCH=arm64 CROSS_COMPILE=<툴체인 절대경로>/bin/aarch64-none-linux-gnu- make -j<코어 개수>
```
<BR>

# 4. 리눅스 실행

QEMU 설치 및 실행

```bash
$ sudo apt-get install qemu-system-aarch64
$ qemu-system-aarch64 \
-kernel <리눅스 디렉토리>/arch/arm64/boot/Image \
-drive format=raw,file=<빌드루트 디렉토리>/output/images/rootfs.ext4,if=virtio \
-append "root=/dev/vda console=ttyAMA0 nokaslr" \
-nographic -M virt -cpu cortex-a72 \
-m 2G \           # 사용할 메모리
-smp 2            # qemu 실행에서 사용할 코어 개수
```
<BR>


빌드한 루트 파일 시스템과 커널 버전을 확인할 수 있다. 

<img width="855" height="399" alt="image" src="https://github.com/user-attachments/assets/f65266fa-93ef-4303-b01c-1b7ddc01a4e9" />

<BR>
<BR>

종료 명령어로 qemu를 벗어날 수 있다. 

```bash
$ shutdown -h now
```
