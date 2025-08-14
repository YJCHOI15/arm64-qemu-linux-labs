// user_charmem.c — 간단한 charmem_buffer 사용자 코드 예제
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>   
#include <unistd.h>   
#include <sys/types.h>
#include <sys/stat.h>

#ifndef CHARDEV_PATH
#define CHARDEV_PATH "/dev/charmem_buffer"   // 드라이버가 생성한 노드명과 일치시킴
#endif

int main(void)
{
    const char *msg = "hello charmem\n";
    char buf[4096] = {0};

    // 1) 쓰기
    int fdw = open(CHARDEV_PATH, O_WRONLY | O_CLOEXEC);
    if (fdw < 0) {
        perror("open(O_WRONLY)");
        return 1;
    }

    ssize_t w = write(fdw, msg, strlen(msg));
    if (w < 0) {
        perror("write");
        close(fdw);
        return 1;
    }
    printf("[write] %zd bytes written\n", w);

    if (close(fdw) < 0) {
        perror("close(w)");
        return 1;
    }

    // 2) 읽기
    int fdr = open(CHARDEV_PATH, O_RDONLY | O_CLOEXEC);
    if (fdr < 0) {
        perror("open(O_RDONLY)");
        return 1;
    }

    ssize_t r = read(fdr, buf, sizeof(buf));
    if (r < 0) {
        perror("read");
        close(fdr);
        return 1;
    }
    printf("[read] %zd bytes read\n", r);

    if (close(fdr) < 0) {
        perror("close(r)");
        return 1;
    }

    // 3) 읽은 내용 일부 출력 (바이너리일 수도 있으니 길이 제한하여 안전하게 표시)
    // 이번 드라이버는 텍스트/바이너리 모두 허용이므로, 데모에선 텍스트라고 가정하고 출력함
    fwrite(buf, 1, (size_t)r, stdout);
    return 0;
}
