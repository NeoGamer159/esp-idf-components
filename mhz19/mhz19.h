#pragma once

#include "driver/uart.h"

#define MHZ19_TX_PIN 17
#define MHZ19_RX_PIN 16
#define MHZ19_UART_NUM UART_NUM_2

void mhz19_init(uart_port_t uart_num);
esp_err_t mhz19_read_co2(uart_port_t uart_num, int *co2);
esp_err_t mhz19_calibrate_zero(uart_port_t uart_num);