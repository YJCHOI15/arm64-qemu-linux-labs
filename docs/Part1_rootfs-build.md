# 빌드루트 (Buildroot)

빌드 루트는 임베디드 리눅스 시스템을 위한 빌드 시스템이다. 

다음과 같은 기능을 한다. 

- 임베디드 리눅스 커널을 컴파일
- 파일 시스템 이미지 생성
- 하드웨어에 맞는 최소화된 루트 파일 시스템 구축

<BR>

# 루트 파일 시스템 (Root File System)

Rootfs는 운영체제에서 모든 파일과 디렉토리의 최상위 티렉토리이다. 

운영체제가 부팅될 때 루트 파일 시스템을 마운트하여 시스템에서 필요한 모든 파일에 접근한다. 

커널, 라이브러리, 애플리케이션, 설정 파일 등이 포함된다. 

임베디드 시스템에서 Buildroot와 같은 도구를 사용하여 최소화된 Rootfs을 만들고, 이를 통해 시스템을 부팅한다. 

<BR>

# 1. 빌드루트 다운로드

```bash
$ git clone https://github.com/buildroot/buildroot.git
```

<BR>

# 2. QEMU 사용 설정

QEMU (Quick Emulator)는 하드웨어 가상화 및 에뮬레이션을 지원하는 오픈소스 소프트웨어이다. 

실제 하드웨어 없이도 가상 환경에서 시스템을 실행하고 테스트할 수 있게 해준다. 

Buildroot를 사용하여 QEMU에서 실행할 aarch64 (64bit-ARM) 시스템 이미지를 만드는 데 필요한 기본 초기 설정을 자동으로 적용한다. 

설정에는 커널, 라이브러리, 애플리케이션 등이 포함된다. 

```bash
$ sudo apt update
$ sudo apt install build-essential
$ sudo apt install make
$ cd buildroot/
$ make qemu_aarch64_virt_defconfig
```

make 후에 `.config`라는 파일이 생긴다. 이 파일에 qemu와 관련된 설정이 들어가 있다. 

<img width="715" height="126" alt="image" src="https://github.com/user-attachments/assets/2c77e87c-29d7-438e-8a2c-1e56bb3355b4" />

<BR>
<BR>

# 3. menuconfig

menuconfig는 Buildroot에서 시스템 설정을 수정할 수 있는 GUI 기반 설정 도구이다. 

커널 모듈, 디바이스 드라이버, 파일 시스템 등을 설정할 수 있다. 

```bash
$ sudo kill -9 16120 # 아래 패키지 설치 안되면 이 명령어 사용
$ sudo apt-get install libncurses-dev
$ make menuconfig
```

<BR>

menuconfig에서 다음과 같은 설정을 해준다. 

1. System configuration → Init system → systemd
    
    → 리눅스 시스템 초기화 및 관리를 systemd로 처리하기 위함
    
2. Kernel → Linux Kernel 선택 해제
    
    → Buildroot에서 기본적으로 제공하는 리눅스 커널을 빌드하지 않고, 별도로 준비된 커널을 사용하기 위함
    
3. Target Packege → Text editors and viewers → vim (옵션)
    
    → 기본 에디터를 vim으로 사용하려면 선택
    
4. Target Package → Libraries → Crypto → Openssl support, openssl binary
    
    → 시스템 보안 기능 활성화, 암호화 통신 및 보안 인증을 처리하기 위함
    
    → Openssl 라이브러리를 바이너리 형태로 포함시켜, 빌드된 시스템에 바로 사용하기 위함
    
5. Filesystem images -> ext2/3/4 root filesystem → ext4
    
    → ext4는 리눅스에서 널리 사용되는 **파일 시스템**
    
6. Filesystem images → exact size → 128M
7. Host utilities → 모든 선택 해제
    
    → `-*-` 는 선택 해제 불가니까 냅두기
    
    → 불필요한 도구와 라이브러리를 제외하여 빌드 시간을 단축하고, 시스템 이미지를 최소화하기 위함
    
<BR>

마지막으로 여러 코어를 사용해 빌드를 해준다. 

```bash
$ nproc --all # 코어 개수 확인
# WSL2를 사용하는 경우 echo $PATH 명령어에서 윈도우 관련 경로를 전부 지우고 
# ~/.bashrc 맨 끝에 export PATH="경로들" 추가
# source ~/.bashrc
$ make -j<코어개수>
```

<BR>

루트파일 시스템 이미지가 생성되고 rootfs.ext4 이미지로 성공적으로 저장되었음을 확인할 수 있다. 

<img width="574" height="87" alt="image" src="https://github.com/user-attachments/assets/8ca14bfb-61ed-4fca-9b3c-e1b3cee1ba23" />
<img width="916" height="140" alt="image" src="https://github.com/user-attachments/assets/1ed321ab-4393-4294-b18a-3df666600d78" />
