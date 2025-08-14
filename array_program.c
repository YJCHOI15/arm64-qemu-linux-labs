#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 시스템 콜 번호 매크로로 정의
#define __NR_set_array 470
#define __NR_get_array 471

int main(int argc, char *argv[]) {
		long i, data, result;
		if (argc == 4 && !strcmp(argv[1], "set_array")) {
				i = atoi(argv[2]);
				data = atoi(argv[3]);
				result = syscall(__NR_set_array, i, data);
				if (result != 0) {
						fprintf(stderr, "Failed to push\n");
				}
		} else if (argc == 3 && !strcmp(argv[1], "get_array")) {
				i = atoi(argv[2]);
				result = syscall(__NR_get_array, i);
				if (result != -1) {
						printf("Value : %ld\n", result);
				} else {
						fprintf(stderr, "Failed to pop\n");
				}
		} else {
				fprintf(stderr, "Usage : %s <set_array | get_array> [index] [data]\n", argv[0]);
					
				return -1;
		}
		return 0;
}