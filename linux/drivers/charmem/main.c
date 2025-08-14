#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/hashtable.h>

#define CHARMEM_DEVICE_NAME  "charmem_buffer"
#define CHARMEM_CLASS_NAME   "charmem"
#define ARRAY_SIZE           4096

static int charmem_device_major;
static struct class *charmem_class;
static struct device *charmem_device;

// 전역 변수 배열 선언
static char charmem_buffer[ARRAY_SIZE];
// 전역 변수 스핀락 선언
static DEFINE_RWLOCK(charmem_splock);

static ssize_t charmem_device_read(struct file *fp, char __user *buf,
                                   size_t len, loff_t *ppos) {
    int read_bytes = 0;

    // 읽을 바이트 수 계산
    if (len >= ARRAY_SIZE) read_bytes = ARRAY_SIZE;
    else                   read_bytes = len;

    // 사용자 공간 버퍼에 전역변수 복사
    read_lock(&charmem_splock);
    if (copy_to_user(buf, charmem_buffer, read_bytes)) {
        return -EFAULT;
    }
    read_unlock(&charmem_splock);

    return read_bytes;
}

static ssize_t charmem_device_write(struct file *fp, const char __user *buf,
                                    size_t len, loff_t *ppos) {
    int written_bytes = 0;

    // 쓸 바이트 수 계산
    if (len >= ARRAY_SIZE) written_bytes = ARRAY_SIZE;
    else                   written_bytes = len;

    // 사용자 공간 버퍼에서 전역변수로 복사
    write_lock(&charmem_splock);
    if (copy_from_user(charmem_buffer, buf, written_bytes)) {
        return -EFAULT;
    }
    write_unlock(&charmem_splock);

    return written_bytes;
}

// struct file_operations: 파일 시스템 호출이 들어왔을 때 
//                         어떤 함수가 실행될지를 연결해 줌
static struct file_operations charmem_device_fops = {
    .owner = THIS_MODULE,           // 이 구조체를 소유하는 모듈 지정 (현재 모듈로 지정)
    .read = charmem_device_read,    // 유저 공간에서 read()을 호출했을 때 실행할 함수
    .write = charmem_device_write,  // 유저 공간에서 write()을 호출했을 때 실행할 함수
};

static int __init charmem_module_init(void) {
    int ret = 0;

    // 주번호 할당 (자동할당을 위해 0부여)
    charmem_device_major = register_chrdev(0, CHARMEM_DEVICE_NAME, &charmem_device_fops);
    if (charmem_device_major < 0) {
        printk(KERN_ERR "%s: Failed to get major number\n", CHARMEM_DEVICE_NAME);
        ret = charmem_device_major;
        goto err_register_chrdev;
    }

    // class 생성
    charmem_class = class_create(CHARMEM_CLASS_NAME);
    if (IS_ERR(charmem_class)) {
        printk(KERN_ERR "%s: Failed to create class\n", CHARMEM_DEVICE_NAME);
        ret = PTR_ERR(charmem_class);
        goto err_class;
    }

    // udev 노드 생성
    charmem_device = device_create(charmem_class, NULL,
                                   MKDEV(charmem_device_major, 0), // 주번호 자동 할당, 부번호 0번
                                   NULL, CHARMEM_DEVICE_NAME);     // /dev/에 생성할 디바이스 파일명
    if (IS_ERR(charmem_device)) {
        ret = PTR_ERR(charmem_device);
        goto err_device;
    }

    return ret;

err_device:
    class_destroy(charmem_class);
err_class:
    unregister_chrdev(charmem_device_major, CHARMEM_DEVICE_NAME);
err_register_chrdev:
    return ret;
}

static void __exit charmem_module_exit(void) {
    device_destroy(charmem_class, MKDEV(charmem_device_major, 0));
    class_destroy(charmem_class);
    unregister_chrdev(charmem_device_major, CHARMEM_DEVICE_NAME);

    printk(KERN_INFO "charmem: module unloaded\n");
}

module_init(charmem_module_init);
module_exit(charmem_module_exit);

MODULE_AUTHOR("YJCHOI15");
MODULE_DESCRIPTION("Simple 4KB character buffer driver");
MODULE_LICENSE("GPL v2");
