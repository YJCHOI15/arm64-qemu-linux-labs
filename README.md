# Embedded Linux & Kernel Engineering Projects

이 레포지토리는 **임베디드 리눅스 환경**에서의 빌드, 분석, 시스템콜 확장, 디바이스 드라이버 구현까지의 전 과정을 다룬 5개의 프로젝트를 포함합니다.
각 프로젝트의 구현 과정은 [docs](https://github.com/YJCHOI15/embedded-linux-kernel/tree/main/docs)를 참고해주세요.

---
## 전체 아키텍쳐 개요
<img width="1410" height="873" alt="image" src="https://github.com/user-attachments/assets/6824a83f-dab3-4d21-ae94-e71a3356bc97" />

---

## 프로젝트 구성

### 1. Buildroot를 이용한 Root Filesystem 빌드
- **목표:** 최소화된 임베디드 리눅스 Rootfs(ext4) 이미지 생성
- **주요 내용:**
  - `make qemu_aarch64_virt_defconfig`를 통한 QEMU aarch64 기본 설정
  - `menuconfig`에서 systemd, OpenSSL, ext4, vim 등 설정
  - 불필요한 Host utilities 제거로 빌드 최적화
- **결과:** `rootfs.ext4` 이미지 생성 후 QEMU에서 부팅 가능

---

### 2. 리눅스 커널 빌드 및 QEMU 실행
- **목표:** ARM64 타겟에 맞춘 커널 빌드 및 Rootfs 연동
- **주요 내용:**
  - Stable Linux 커널 다운로드
  - ARM GNU Toolchain 사용 (`aarch64-none-linux-gnu-`)
  - Buildroot의 QEMU 설정 파일(`linux.config`)을 `qemu_defconfig`로 복사
  - `ARCH=arm64 make qemu_defconfig` 및 `CROSS_COMPILE`로 빌드
- **결과:** QEMU에서 커널 + Rootfs 부팅 및 동작 확인

---

### 3. Strace를 사용한 ls 명령어 분석
- **목표:** `strace`를 통해 명령어 내부 동작 원리와 시스템콜 흐름 분석
- **주요 분석 포인트:**
  - `openat()`, `getdents64()`, `write()`가 핵심 동작
  - `mmap()`이 라이브러리 로딩과 버퍼 할당에 빈번히 사용됨
  - 실행 준비 과정에서 `/etc/ld.so.cache`, `/lib/...`, `/proc/filesystems` 등의 파일 접근
- **결론:** 단순 명령어도 다양한 시스템콜과 커널 상호작용을 통해 동작

---

### 4. 커스텀 시스템콜 추가 (32-bit Integer Array Access)
- **목표:** `set_array()` / `get_array()` 시스템콜 구현
- **주요 내용:**
  - 커널 소스에 `array_syscalls.c` 추가 후 `Makefile`/`syscall_64.tbl` 수정
  - 최대 64개의 32-bit 정수 배열에 접근 가능
  - User-space 프로그램(`array_program`) 작성 및 QEMU Rootfs에 배포
- **결과:** QEMU에서 커스텀 시스템콜 호출 테스트 성공

---

### 5. 커널 모듈 기반 Charmem 디바이스 드라이버 구현 (In-Tree 방식)
- **목표:** `/dev/charmem_buffer` 문자 디바이스 생성 및 read/write 동작 구현
- **주요 내용:**
  - LKM 형태로 작성, udev를 통한 자동 디바이스 노드 생성
  - 4KB 전역 버퍼 기반 데이터 저장/읽기 처리
  - 사용자 프로그램(`user_charmem`) 작성 및 Rootfs에 배포
- **결과:** QEMU 환경에서 드라이버 로드 후 사용자 프로그램과 정상 데이터 송수신

---

## 기술 스택
- **개발 환경:** Ubuntu-20.04(WSL2), vscode, QEMU (ARM64), Buildroot
- **언어:** C (커널 & 사용자 프로그램)
- **툴체인:** ARM GNU Toolchain (`aarch64-none-linux-gnu-`)
