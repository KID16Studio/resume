#include <stdio.h>    // 標準輸入輸出函數
#include <stdlib.h>   // 標準庫函數
#include <fcntl.h>    // 文件控制選項
#include <unistd.h>   // UNIX 標準函數
#include <time.h>     // 時間相關函數
#include <signal.h>   // 信號處理
#include <string.h>   // 字符串處理
#include <errno.h>    // 錯誤號
#include <poll.h>     // 輪詢函數

#define DEVICE_FILE "/dev/motion_sensor"  // 定義設備文件路徑
#define BUFFER_SIZE 16  // 定義緩衝區大小

volatile sig_atomic_t keep_running = 1;  // 控制程序運行的標誌

void signal_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM || signo == SIGQUIT) {
        printf("Preparing to exit...\n");
        keep_running = 0;  // 設置標誌以結束主循環
    }
}

void set_user_space_running(int fd, int state) {
    if (fd < 0) {
        fprintf(stderr, "Invalid file descriptor\n");
        return;
    }
    // 向設備寫入用戶空間程序運行狀態
    if (write(fd, &state, sizeof(state)) < 0) {
        perror("Error setting user space running state");
    }
}

void take_a_snapshot(const char* filename, int delay, int width, int height) {
    char command[256];
    snprintf(command, sizeof(command), "raspistill -o %s -t %d -w %d -h %d", filename, delay, width, height);

    FILE* fp = popen(command, "r");  // 執行拍照命令
    if (fp == NULL) {
        perror("popen failed");
        return;
    }
    pclose(fp);  // 關閉管道
}


int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    time_t now;
    struct pollfd pfd;

    printf("Motion sensor program starting...\n");

    // 設置信號處理
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1 || 
        sigaction(SIGTERM, &sa, NULL) == -1 ||
        sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("Unable to set signal handler");
        return 1;
    }

    fd = open(DEVICE_FILE, O_RDWR);   // 以讀寫方式打開設備文件
    if (fd < 0) {
        perror("Unable to open device file");
        return 1;
    }

    // 通知內核模塊用戶空間程序正在運行
    set_user_space_running(fd, 1);

    pfd.fd = fd;
    pfd.events = POLLIN;

    printf("Successfully opened device file, entering main loop\n");

    while (keep_running) {
        int poll_result = poll(&pfd, 1, 500);   // 0.5 秒超時

        if (poll_result < 0) {
            if (errno != EINTR) {
                perror("Poll error");
                break;
            }
        } else if (poll_result > 0) {
            if (pfd.revents & POLLIN) {
                int bytes_read = read(fd, buffer, BUFFER_SIZE);
                if (bytes_read <= 0) {
                    if (bytes_read < 0) {
                        perror("Error reading from device");
                    }
                    continue;
                }
                if (buffer[0] == '1') {
                    // 檢測到運動，打開 LED
                    time(&now);
                    printf("Motion detected! Time: %s", ctime(&now));
                    take_a_snapshot("../application/public/captured_image.jpg", 1000, 640, 480);
                }
            }
        } else {
            // 如果沒有檢測到運動，保持 LED 關閉
            printf("No movement detected\n");
        }
    }

    // 通知內核模塊用戶空間程序正在停止
    set_user_space_running(fd, 0);

    if (close(fd) < 0) {
        perror("Error closing device file");
    }
    printf("Program exiting normally\n");
    return 0;
}