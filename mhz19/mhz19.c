#include "mhz19.h"

void mhz19_init(uart_port_t uart_num) {
    // Initialize UART communication with the MH-Z19 sensor
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, MHZ19_TX_PIN, MHZ19_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, 256, 0, 0, NULL, 0);
}

esp_err_t mhz19_read_co2(uart_port_t uart_num, int *co2) {
    uint8_t cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    uart_write_bytes(uart_num, (const char *)cmd, sizeof(cmd));
    
    uint8_t response[9];
    int len = uart_read_bytes(uart_num, response, sizeof(response), pdMS_TO_TICKS(1000));
    
    //Check lenght and header
    if (len != 9 || response[0] != 0xFF || response[1] != 0x86) {
        return ESP_FAIL;
    }
    
    //Data validation - checksum
    uint8_t checksum = 0;
    for (int i = 1; i < 8; i++) {
        checksum += response[i];
    }
    checksum = (~checksum + 1) & 0xFF;
    if (checksum != response[8]) {
        return ESP_FAIL;
    }
    
    // Calculate CO2 concentration
    *co2 = (response[2] << 8) | response[3];
    return ESP_OK;
}

// Calibrate zero point of the sensor
esp_err_t mhz19_calibrate_zero(uart_port_t uart_num) {
    uint8_t cmd[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
    int written = uart_write_bytes(uart_num, (const char *)cmd, sizeof(cmd));
    
    if (written != sizeof(cmd)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}