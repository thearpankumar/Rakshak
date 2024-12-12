#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <stdlib.h> // For rand() and srand()

/* Timer handler declaration */
void send_timer_handler(struct k_timer *timer);

/* Define a periodic timer */
K_TIMER_DEFINE(send_timer, send_timer_handler, NULL);

/* Select the UART peripheral */
#define UART_DEVICE_NODE DT_CHOSEN(rakshak_uart1)

/* Message queue settings */
#define MSG_SIZE 32
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

/* Get the UART device */
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* UART receive buffer */
static char rx_buf[MSG_SIZE];
static int rx_buf_pos = 0;

/* UART interrupt callback */
void serial_cb(const struct device *dev, void *user_data) {
    uint8_t c;

    if (!uart_irq_update(dev)) {
        return;
    }

    if (uart_irq_rx_ready(dev)) {
        while (uart_fifo_read(dev, &c, 1) == 1) {
            if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
                rx_buf[rx_buf_pos] = '\0';
                k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);
                rx_buf_pos = 0;
            } else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
                rx_buf[rx_buf_pos++] = c;
            }
        }
    }
}

/* Print a string via UART */
void print_uart(const char *buf) {
    while (*buf) {
        uart_poll_out(uart_dev, *buf++);
    }
}

/* Send a number as a string via UART */
void send_number(int num) {
    char str_num[10];
    snprintf(str_num, sizeof(str_num), "%d\r\n", num);
    print_uart(str_num);
}

/* Timer handler to send random numbers */
void send_timer_handler(struct k_timer *timer) {
    int random_number = (rand() % 41) + 80; // Generate random number between 80 and 120
    send_number(random_number);
    printk("Sent: Heart-Beat: %d\n", random_number);
}

/* Main function */
int main(void) {
    char rx_buf_main[MSG_SIZE];

    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready!\n");
        return 0;
    }

    /* Set UART callback */
    if (uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL) < 0) {
        printk("Failed to set UART callback\n");
        return 0;
    }

    uart_irq_rx_enable(uart_dev);

    /* Seed the random number generator */
    srand(k_cycle_get_32()); // Use a unique value (e.g., system cycle count)

    /* Start the periodic timer */
    k_timer_start(&send_timer, K_SECONDS(1), K_SECONDS(1));

    print_uart("Starting random number sender...\r\n");

    while (1) {
        if (k_msgq_get(&uart_msgq, &rx_buf_main, K_MSEC(1000)) == 0) {
            print_uart("Received: ");
            print_uart(rx_buf_main);
            print_uart("\r\n");
        }
    }

    return 0;
}
