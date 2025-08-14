#include <linux/kernel.h>
#include <linux/syscalls.h>

#define ARRAY_SIZE (256 / sizeof(uint32_t))

static uint32_t array[ARRAY_SIZE] = {0};

SYSCALL_DEFINE2(set_array, uint32_t, i, uint32_t, data) {
		if (i >= ARRAY_SIZE) return -1;
		array[i] = data;
		return 0;
}

SYSCALL_DEFINE1(get_array, uint32_t, i) {
		if (i >= ARRAY_SIZE) return -1;
		return array[i];
}