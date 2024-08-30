#include <linux/module.h>      // 包含基本的內核模塊功能
#include <linux/kernel.h>      // 包含內核相關的函數和結構體
#include <linux/init.h>        // 包含模塊初始化和清理函數的宏
#include <linux/fs.h>          // 包含文件系統相關的函數和結構體
#include <linux/uaccess.h>     // 提供用戶空間和內核空間之間的數據傳輸函數
#include <linux/gpio.h>        // 包含 GPIO 操作相關的函數
#include <linux/interrupt.h>   // 包含中斷處理相關的函數和結構體
#include <linux/poll.h>        // 包含輪詢操作相關的函數和結構體
#include <linux/delay.h>       // 包含延遲函數

#define DEVICE_NAME "motion_sensor"  // 定義設備名稱
#define SENSOR_GPIO 17               // 定義傳感器使用的 GPIO 引腳號
#define LED_GPIO 18                  // 定義 LED 使用的 GPIO 引腳號
#define LED_ON_TIME_MS 700           // 定義 LED 亮起的時間（毫秒）

static int major_number;             // 存儲主設備號
static struct class *motion_sensor_class;    // 設備類別
static struct device *motion_sensor_device;  // 設備結構體
static int motion_detected;          // 運動檢測標誌
static int user_space_running;       // 用戶空間程序運行標誌
static DECLARE_WAIT_QUEUE_HEAD(motion_waitqueue);  // 聲明等待隊列

static irqreturn_t motion_interrupt(int irq, void *dev_id) {
    if (user_space_running) {  // 只有在用戶空間程序運行時才處理中斷
        motion_detected = 1;   // 設置運動檢測標誌
        wake_up_interruptible(&motion_waitqueue);  // 喚醒等待的進程
        gpio_set_value(LED_GPIO, 1);  // 打開 LED
        mdelay(LED_ON_TIME_MS);       // 延遲指定時間
        gpio_set_value(LED_GPIO, 0);  // 關閉 LED
        pr_info("Motion detected!\n");  // 輸出調試信息
    }
    return IRQ_HANDLED;  // 表示中斷已處理
}

static unsigned int device_poll(struct file *file, poll_table *wait) {
    poll_wait(file, &motion_waitqueue, wait);  // 添加等待隊列到 poll 表
    return motion_detected ? (POLLIN | POLLRDNORM) : 0;  // 返回可讀狀態
}

static int device_open(struct inode *inode, struct file *file) {
    return 0;  // 簡單的打開操作，不需要特殊處理
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;  // 簡單的關閉操作，不需要特殊處理
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    char message[2] = {motion_detected ? '1' : '0', '\n'};  // 準備返回的消息
    motion_detected = 0;  // 重置運動檢測狀態

    if (len < 2)
        return -EINVAL;  // 如果提供的緩衝區太小，返回錯誤

    if (copy_to_user(buffer, message, 2))  // 將數據複製到用戶空間
        return -EFAULT;  // 如果複製失敗，返回錯誤

    return 2;  // 返回寫入的字節數
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    if (len != sizeof(int)) {
        return -EINVAL;  // 如果寫入的數據大小不正確，返回錯誤
    }

    if (copy_from_user(&user_space_running, buffer, sizeof(user_space_running))) {
        return -EFAULT;  // 如果從用戶空間複製數據失敗，返回錯誤
    }

    if (user_space_running) {
        gpio_set_value(LED_GPIO, 0);  // 用戶空間程序啟動時確保 LED 關閉
    } else {
        gpio_set_value(LED_GPIO, 0);  // 用戶空間程序停止時關閉 LED
    }

    return sizeof(user_space_running);  // 返回寫入的字節數
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .poll = device_poll,
};  // 定義文件操作結構體

static int __init motion_sensor_init(void) {
    int ret;

    // 註冊字符設備
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Failed to register character device\n");
        return major_number;
    }

    // 創建設備類
    motion_sensor_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(motion_sensor_class)) {
        pr_err("Failed to create device class\n");
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(motion_sensor_class);
    }

    // 創建設備
    motion_sensor_device = device_create(motion_sensor_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(motion_sensor_device)) {
        pr_err("Failed to create the device\n");
        class_destroy(motion_sensor_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(motion_sensor_device);
    }

    // 請求 GPIO
    gpio_request_one(SENSOR_GPIO, GPIOF_IN, "motion_sensor");
    gpio_request_one(LED_GPIO, GPIOF_OUT_INIT_LOW, "led");  // 初始化 LED 為低電平

    // 請求中斷
    ret = request_irq(gpio_to_irq(SENSOR_GPIO), motion_interrupt, IRQF_TRIGGER_RISING, "motion_sensor", NULL);
    if (ret) {
        pr_err("Unable to request IRQ\n");
        device_destroy(motion_sensor_class, MKDEV(major_number, 0));
        class_destroy(motion_sensor_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return ret;
    }

    pr_info("Motion sensor module initialized\n");
    return 0;
}

static void __exit motion_sensor_exit(void) {
    free_irq(gpio_to_irq(SENSOR_GPIO), NULL);  // 釋放中斷
    gpio_set_value(LED_GPIO, 0);  // 確保退出時 LED 關閉
    gpio_free(LED_GPIO);  // 釋放 LED GPIO
    gpio_free(SENSOR_GPIO);  // 釋放傳感器 GPIO
    device_destroy(motion_sensor_class, MKDEV(major_number, 0));  // 銷毀設備
    class_destroy(motion_sensor_class);  // 銷毀設備類
    unregister_chrdev(major_number, DEVICE_NAME);  // 註銷字符設備
    pr_info("Motion sensor module unloaded\n");
}

module_init(motion_sensor_init);  // 註冊模塊初始化函數
module_exit(motion_sensor_exit);  // 註冊模塊退出函數
MODULE_LICENSE("GPL");  // 聲明模塊的許可證